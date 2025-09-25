#include <vector>
#include <Eigen/Core>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int iterNum = 100;
double thres = 0.5;
void fitLineGaussionNewTown(const vector<Vector2d>& points, Vector3d& initial) {
    for (int i = 0; i < iterNum; i++) {
        Vector3d Jaco(Vector3d::Zero());
        Matrix3d Hassion(Matrix3d::Zero());
        Vector3d g(Vector3d::Zero());
        double cost = 0;
        double a = initial(0), b = initial(1), c = initial(2);
        for (int j = 0; j < points.size(); j++) {
            double x = points[j].x();
            double y = points[j].y();
            double res = y - a * x * x - b * x - c;
            Vector3d gradiant{ - x * x, - x, -1 };
            Matrix3d hass = gradiant * gradiant.transpose();
            Jaco += gradiant;
            Hassion += hass;
            g += - gradiant * res;
            cost = 0.5 * res * res;
        }
        Vector3d delta = Hassion.ldlt().solve(g);
        initial += delta;
        if (cost < thres) break;
    }
}

int main(int argc, char const *argv[])
{
    
    return 0;
}
