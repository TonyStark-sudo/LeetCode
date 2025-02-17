// Author: zhangsongpeng

#include "base/camera_calib.h"

#include "util/misc.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace colmap {

CameraCalib::CameraCalib() {}

size_t CameraCalib::NumCameras() const { return calib_cameras_.size(); }

bool CameraCalib::HasCamera(const camera_t camera_id) const {
  return calib_cameras_.count(camera_id);
}

std::vector<camera_t> CameraCalib::GetCameraIds() const {
  std::vector<camera_t> calib_camera_ids;
  calib_camera_ids.reserve(NumCameras());
  for (const auto& calib_camera : calib_cameras_) {
    calib_camera_ids.push_back(calib_camera.first);
  }
  return calib_camera_ids;
}

void CameraCalib::AddCamera(const camera_t camera_id,
                            const Eigen::Vector4d& rel_qvec,
                            const Eigen::Vector3d& rel_tvec,
                            const double rel_time,
                            const std::string& camera_name) {
  CHECK(!HasCamera(camera_id));
  CalibCamera calib_camera;
  calib_camera.rel_qvec = rel_qvec;
  calib_camera.rel_tvec = rel_tvec;
  calib_camera.rel_time = rel_time;
  calib_camera.qvec_prior = rel_qvec;
  calib_camera.tvec_prior = rel_tvec;
  calib_camera.time_prior = rel_time;
  calib_camera.name = camera_name;
  calib_cameras_.emplace(camera_id, calib_camera);
}

void CameraCalib::Check(const Reconstruction& reconstruction) const {
  for (const auto& calib_camera : calib_cameras_) {
    CHECK(reconstruction.ExistsCamera(calib_camera.first));
  }
}

Eigen::Vector4d& CameraCalib::RelativeQvec(const camera_t camera_id) {
  return calib_cameras_.at(camera_id).rel_qvec;
}

const Eigen::Vector4d& CameraCalib::RelativeQvec(const camera_t camera_id) const {
  return calib_cameras_.at(camera_id).rel_qvec;
}

Eigen::Vector3d& CameraCalib::RelativeTvec(const camera_t camera_id) {
  return calib_cameras_.at(camera_id).rel_tvec;
}

const Eigen::Vector3d& CameraCalib::RelativeTvec(const camera_t camera_id) const {
  return calib_cameras_.at(camera_id).rel_tvec;
}

double& CameraCalib::RelativeTime(const camera_t camera_id) {
  return calib_cameras_.at(camera_id).rel_time;
}

const double& CameraCalib::RelativeTime(const camera_t camera_id) const {
  return calib_cameras_.at(camera_id).rel_time;
}

void CameraCalib::Read(const std::string& path) {
  if (ExistsFile(path)) {
    ReadCalibJson(path);
  } else {
    LOG(FATAL) << "key_frame files do not exist at " << path;
  }
}

void CameraCalib::Write(const std::string& path) const { WriteCalibJson(path); }

void CameraCalib::ReadCalibJson(const std::string& path) {
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(path, pt);

  calib_cameras_.clear();
  for (const auto& camera : pt) {
    const int camera_id = camera.second.get<int>("camera_id");
    Eigen::Vector3d rel_tvec;
    Eigen::Vector4d rel_qvec;
    int index = 0;
    auto rel_tvec_node = camera.second.get_child("rel_tvec");
    for (const auto& node : rel_tvec_node) {
      rel_tvec[index++] = node.second.get_value<double>();
    }
    index = 0;
    auto rel_qvec_node = camera.second.get_child("rel_qvec");
    for (const auto& node : rel_qvec_node) {
        rel_qvec[index++] = node.second.get_value<double>();
    }

    const double rel_time = camera.second.get<double>("rel_time");
    const auto camera_name = camera.first;
    AddCamera(camera_id, rel_qvec, rel_tvec, rel_time, camera_name);
    rel_time_ = rel_time;
  }
}

void CameraCalib::WriteCalibJson(const std::string& path) const {
  boost::property_tree::ptree pt;
  for (const auto& camera : calib_cameras_) {
    boost::property_tree::ptree camera_node;
    camera_node.put("camera_id", camera.first);
    boost::property_tree::ptree tvec;
    for (int j = 0; j < 3; j++) {
      boost::property_tree::ptree cell;
      cell.put_value(camera.second.rel_tvec[j]);
      tvec.push_back(std::make_pair("", cell));
    }
    camera_node.add_child("rel_tvec", tvec);

    boost::property_tree::ptree qvec;
    const Eigen::Vector4d rel_qvec = NormalizeQuaternion(camera.second.rel_qvec);
    for (int j = 0; j < 4; j++) {
      boost::property_tree::ptree cell;
      cell.put_value(rel_qvec[j]);
      qvec.push_back(std::make_pair("", cell));
    }
    camera_node.add_child("rel_qvec", qvec);

    camera_node.put("rel_time", rel_time_);

    pt.add_child(camera.second.name, camera_node);
  }
  boost::property_tree::write_json(path, pt);
}

void CameraCalib::PrintInfo() const {
  std::cout << "---------------- CameraCalib::PrintInfo !!! ----------------" << std::endl;
  for (const auto& camera : calib_cameras_) {
    std::cout << std::setprecision(6) << "cam id: " << camera.first << std::endl;
    std::cout << "dt: " << camera.second.time_prior << ", " << camera.second.rel_time << std::endl;
    std::cout << "trans: " << camera.second.tvec_prior.transpose() << ", " << camera.second.rel_tvec.transpose() << std::endl;
    std::cout << "quat: " << camera.second.qvec_prior.transpose() << ", " << camera.second.rel_qvec.transpose() << std::endl;
  }
  std::cout << "dt: " << time_prior_ << ", " << rel_time_ << std::endl;
  std::cout << "---------------- CameraCalib::PrintInfo !!! ----------------" << std::endl;
}

}  // namespace colmap
