#include "project.h"
#include "project_cuda.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <opencv2/opencv.hpp>

Projector::Projector(const std::string &config_path) {
    std::ifstream infile(config_path);
    if (infile.is_open()) {
        infile >> config_;
    }
}

float* Projector::mat3x3_to_pointer(const Eigen::Matrix3d& Mat) {
    float* arr = new float[9];
    arr[0] = Mat(0, 0);
    arr[1] = Mat(0, 1);
    arr[2] = Mat(0, 2);

    arr[3] = Mat(1, 0);
    arr[4] = Mat(1, 1);
    arr[5] = Mat(1, 2);

    arr[6] = Mat(2, 0);
    arr[7] = Mat(2, 1);
    arr[8] = Mat(2, 2);

    return arr;
}

float* Projector::vec3x1_to_pointer(const Eigen::Vector3d& vec) {
    float* arr = new float[3];
    arr[0] = vec(0);
    arr[1] = vec(1);
    arr[2] = vec(2);

    return arr;
}

std::vector<PoseStamped> Projector::ReadPoses(const std::string &pose_path) {
    std::vector<PoseStamped> image_poses;   
    std::ifstream infile(pose_path);
    if (!infile.is_open()) {
        std::cout << pose_path << " open failed" << std::endl;
        return {};
    }
    std::string line;
    while (getline(infile, line)) {
        std::stringstream ss(line);
        std::string timestamp;
        ss >> timestamp;
        std::string x;
        std::vector<double> line_vec;
        while (ss >> x) {
            line_vec.push_back(std::stod(x));
        }
        Eigen::Vector3d position(line_vec[0], line_vec[1], line_vec[2]);
        Eigen::Quaterniond rotation(line_vec[6], line_vec[3], line_vec[4], line_vec[5]);
        Pose pose_wc(position, rotation);
        image_poses.emplace_back(std::stod(timestamp), timestamp, pose_wc.Inverse());
    }
    return std::move(image_poses);
}

void Projector::Project() {
    std::string lidar_name = config_["lidar_names"][0];
    std::filesystem::path local_path = std::string(config_["local_path"]);
    std::filesystem::path map_pcd = JoinPaths(local_path, config_["hdmapping_ground_subdir_" + lidar_name], "intensity_static_all.pcd");
    if (!std::filesystem::exists(map_pcd)) {
        std::cout << map_pcd << " not exists" << std::endl;
        return;
    }
    
    std::cout << "begin project" << std::endl;
    pcl::PointCloud<pcl::PointXYZI>::Ptr points_w(new pcl::PointCloud<pcl::PointXYZI>());
    if (pcl::io::loadPCDFile(map_pcd.string(), *points_w) == -1) {
        std::cout << "no pcd: " << map_pcd.string() << std::endl;
        return;
    }

    // ProjectorCUDA(float *points_w, size_t *point_size);
    size_t *point_size = new size_t(points_w->size());
    std::cout << "point_size: " << *point_size << std::endl;
    // *point_size = static_cast<size_t>(points_w->size());
    float *points = new float[4 * points_w->size()];
    for (size_t i = 0; i < points_w->size(); i++) {
        points[4 * i] = points_w->points[i].x;
        points[4 * i + 1] = points_w->points[i].y;
        points[4 * i + 2] = points_w->points[i].z;
        points[4 * i + 3] = points_w->points[i].intensity;
    }

    ProjectorCUDA projector(points, point_size);

    for (const auto &cam_i : config_["camera_names"]) {
        nlohmann::json camera_info;
        std::string cam = cam_i;
        std::ifstream intrinsic_infile(JoinPaths(local_path, config_[cam + "_intrinsic_subpath"]));
        if (!intrinsic_infile.is_open()) {
            continue;
        }
        intrinsic_infile >> camera_info;

        int width = camera_info["resize"][0];
        int height = camera_info["resize"][1];
        Eigen::Matrix3d K;
        K << camera_info["K"][0][0], camera_info["K"][0][1], camera_info["K"][0][2],
             camera_info["K"][1][0], camera_info["K"][1][1], camera_info["K"][1][2],
             camera_info["K"][2][0], camera_info["K"][2][1], camera_info["K"][2][2];
        if (camera_info["resolution"][0] != camera_info["resize"][0]) {
            double scale_w = (double)camera_info["resize"][0] / (double)camera_info["resolution"][0];
            double scale_h = (double)camera_info["resize"][1] / (double)camera_info["resolution"][1];
            K.row(0) *= scale_w;
            K.row(1) *= scale_h;
        }
        float *h_K = mat3x3_to_pointer(K);
        float *h_D = new float[4];
        if (camera_info["undistortion"]) {
            h_D[0] = 0.0;
            h_D[1] = 0.0;
            h_D[2] = 0.0;
            h_D[3] = 0.0;
        }
        else {
            h_D[0] = camera_info["D"][0][0];
            h_D[1] = camera_info["D"][1][0];
            h_D[2] = camera_info["D"][2][0];
            h_D[3] = camera_info["D"][3][0];
        }
        // utility.write_txt(os.path.join(cfg["local_path"], cfg["vl_project_subdir"], f"image_poses_{cam}.txt"), image_poses_list)
        std::filesystem::path image_poses_path = JoinPaths(local_path, config_["vl_project_subdir"], "image_poses_" + cam + ".txt");
        std::filesystem::path image_rgb_dir = JoinPaths(local_path, config_[cam + "_data_subdir"]);
        std::vector<PoseStamped> posest_cw = ReadPoses(image_poses_path);
        auto start = std::chrono::high_resolution_clock::now();
        // for (const auto &poset_cw : posest_cw) {
        for (size_t i = 0; i < posest_cw.size(); i++) {
            const auto &poset_cw = posest_cw[i];
            // std::cout << "\rProgress: " << std::setw(3) << i + 1 << "/" << posest_cw.size();
            std::string timestamp = poset_cw.timestamp_str;
            const Pose &pose_cw = poset_cw.pose;
            float *rotation = mat3x3_to_pointer(pose_cw.q.toRotationMatrix());
            float *translation = vec3x1_to_pointer(pose_cw.p);
            float *depth_image = new float[width * height];
            unsigned char *intensity_image = new unsigned char[width * height];
            projector.project(rotation, translation, &width, &height, h_K, depth_image, intensity_image);
            cv::Mat depth_image_mat = cv::Mat::zeros(height, width, CV_32FC1);
            cv::Mat intensity_image_mat = cv::Mat::zeros(height, width, CV_8UC1);
            std::filesystem::path depth_image_path = JoinPaths(local_path, config_[cam + "_project_depth_subdir"], timestamp + ".jpg");
            std::filesystem::path depth_binary_path = JoinPaths(local_path, config_[cam + "_project_depth_subdir"], timestamp + ".npy");
            std::filesystem::path intensity_image_path = JoinPaths(local_path, config_[cam + "_project_intensity_subdir"], timestamp + ".jpg");
            std::filesystem::path gray_image_path = JoinPaths(local_path, config_[cam + "_project_gray_subdir"], timestamp + ".jpg");
            for (size_t x = 0; x < width; x++) {
                for (size_t y = 0; y < height; y++) {
                    depth_image_mat.at<float>(y, x) = depth_image[y * width + x];
                    intensity_image_mat.at<uchar>(y, x) = intensity_image[y * width + x];
                }
            }

            // std::cout << "depth: " << depth_image_path << std::endl;
            std::cout << "depth: " << depth_image_path << std::endl;
            cv::Mat depth_normalized;
            double min_val, max_val;
            cv::minMaxLoc(depth_image_mat, &min_val, &max_val);  // 获取深度图的最小最大值
            depth_image_mat.convertTo(depth_normalized, CV_8UC1, 255.0 / (max_val - min_val), -min_val * 255.0 / (max_val - min_val));

            cv::Mat depth_colored;
            cv::applyColorMap(depth_normalized, depth_colored, cv::COLORMAP_JET);
            cv::imwrite(depth_image_path, depth_colored);
            // cv::imwrite(depth_path, depth_image_mat);
            std::ofstream file(depth_binary_path, std::ios::binary);
            file.write(reinterpret_cast<const char*>(depth_image_mat.data), depth_image_mat.total() * sizeof(float));

            cv::Mat color_intensity;
            cv::applyColorMap(intensity_image_mat, color_intensity, cv::COLORMAP_JET);
            cv::imwrite(intensity_image_path, color_intensity);

            std::filesystem::path image_rgb_path = JoinPaths(image_rgb_dir, timestamp + ".jpg");
            cv::Mat image_rgb = cv::imread(image_rgb_path, cv::IMREAD_UNCHANGED);
            cv::Mat image_gray;
            cv::cvtColor(image_rgb, image_gray, cv::COLOR_BGR2GRAY);
            cv::imwrite(gray_image_path, image_gray);

            delete [] rotation;
            delete [] translation;
            delete [] depth_image;
            delete [] intensity_image;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::endl << cam << " images size: " << posest_cw.size() << ", cost: " << std::chrono::duration<double>(end - start).count() << " seconds" << std::endl;
        delete [] h_K;
        delete [] h_D;
    }
    delete point_size;
}
