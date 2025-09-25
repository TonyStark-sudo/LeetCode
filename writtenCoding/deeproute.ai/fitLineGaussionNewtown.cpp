#include <random>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>

// y = 3x^2 + 2x + 4
typedef Eigen::Vector2d Point;

std::vector<Point> GenRandomPoints(size_t num, double sigma) {
    std::vector<Point> points;
    std::default_random_engine generator;
    std::normal_distribution<double> noise(0.0, sigma);

    for (size_t i = 0; i < num; i++) {
        double x = static_cast<double>(i);
        double y = 3 * i * i + 2 * i + 4;
        y += noise(generator);
        points.push_back(Point(x, y));
    }
    return points;
}

void fitLineGaussionNewtown(Eigen::Vector3d& initial, const std::vector<Point>& points, double thres, int iterNum) {
    for (int i = 0; i < iterNum; i++) {
        Eigen::Vector3d jaco{Eigen::Vector3d::Zero()};
        Eigen::Matrix3d hassion{Eigen::Matrix3d::Zero()};
        Eigen::Vector3d g{Eigen::Vector3d::Zero()};
        double cost = 0;
        double residual = 0;
        double a = initial.x(), b = initial.y(), c = initial.z();
        for (int j = 0; j < points.size(); j++) {
            double y = points[j].y(), x = points[j].x();
            double tmp_residual = y - a * x * x - b * x - c;
            Eigen::Vector3d jaco_tmp{ - x * x, - x, -1};
            Eigen::Matrix3d hassion_tmp = jaco_tmp * jaco_tmp.transpose();
            jaco += jaco_tmp;
            hassion += hassion_tmp;
            cost += 0.5 * tmp_residual * tmp_residual;
            g += - tmp_residual * jaco_tmp;
        }
        Eigen::Vector3d delta = hassion.ldlt().solve(g);
        initial += delta;
        if (cost < thres) {
            break;
        }
    std::cout << "iter: " << i << " cost: " << cost << " abc: " << initial.transpose() << std::endl;
    }    
}

int main() {
    // 生成带噪声的点
    size_t num_points = 100;
    double noise_sigma = 10.0;
    std::vector<Point> points = GenRandomPoints(num_points, noise_sigma);

    // 初始参数 [a, b, c]
    Eigen::Vector3d initial(0.0, 0.0, 4);

    // 拟合
    double threshold = 1e-6;
    int max_iter = 100;
    fitLineGaussionNewtown(initial, points, threshold, max_iter);

    std::cout << "拟合结果: a=" << initial[0] << ", b=" << initial[1] << ", c=" << initial[2] << std::endl;
    // 理论值应接近 a=3, b=2, c=4
    return 0;
}