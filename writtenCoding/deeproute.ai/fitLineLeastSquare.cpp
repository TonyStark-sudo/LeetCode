#include <Eigen/Dense>
#include <vector>
#include <iostream>

typedef Eigen::Vector2d Point;

Eigen::Vector3d leastSquaresFit(const std::vector<Point>& points) {
    int n = points.size();
    Eigen::MatrixXd A(n, 3);
    Eigen::VectorXd Y(n);
    for (int i = 0; i < n; ++i) {
        double x = points[i].x();
        double y = points[i].y();
        A(i, 0) = x * x;
        A(i, 1) = x;
        A(i, 2) = 1.0;
        Y(i) = y;
    }
    // 最小二乘解
    Eigen::Vector3d result = (A.transpose() * A).ldlt().solve(A.transpose() * Y);
    return result;
}