#include <Eigen/Dense>
#include <Eigen/Core>

#include <vector>
#include <iostream>

class GaussionNewtownFitor {
private:
    int iteration_num_;
    double thres_;

public:
    GaussionNewtownFitor(const int& iteration_num, const double thres) : iteration_num_(iteration_num), 
                        thres_(thres) {};
    void Solver(Eigen::Vector3d& initial_coff, const std::vector<Eigen::Vector2d>& points) {
        std::cout << "Initial curve coff: "; 
        for (int i{0}; i < initial_coff.size(); i++) {
            std::cout << initial_coff[i] << " ";
        }
        std::cout << std::endl;
        for (int i = 0; i < iteration_num_; i++) {
            Eigen::Vector3d Jacobi{Eigen::Vector3d::Zero()};
            Eigen::Matrix3d Hassion{Eigen::Matrix3d::Zero()};
            Eigen::Vector3d g{Eigen::Vector3d::Zero()};
            double cost = 0;
            double a = initial_coff[0], b = initial_coff[1], c = initial_coff[2];
            for (int j = 0; j < points.size(); j++) {
                double error = points[j].y() - (a * points[j].x() * points[j].x() + b * points[j].x() + c);
                Eigen::Vector3d gradiant{-points[j].x() * points[j].x(), -points[j].x(), -1};
                Eigen::Matrix3d hassion = gradiant * gradiant.transpose(); 
                Jacobi += gradiant;
                Hassion += hassion;
                g += -error * gradiant;
                cost += error * error;
            }
            Eigen::Vector3d delta = Hassion.ldlt().solve(g);
            initial_coff += delta;
            if (delta.norm() < thres_) {
                break;
            }
            std::cout << "iter " << i << ": cost = " << cost << ", delta = " << delta.transpose() << std::endl;
        }
    }
    
};

int main(int argc, char const *argv[])
{
    
    return 0;
}
