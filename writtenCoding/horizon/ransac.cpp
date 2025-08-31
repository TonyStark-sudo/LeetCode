#include <vector>
#include <cmath>

using namespace std;

struct Point {
    double x, y;
};

class Circle {
public:
    Point center;
    double radius;
    Circle() : center({0., 0.}), radius(0.) {};
    Circle(Point c, double r) : center(c), radius(r) {};
};

double calcDis(const Point& p1, const Point& p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

Circle calCircle(const Point& p1, const Point& p2, const Point& p3) {   
    // 计算三点确定的圆
    double x1 = p1.x, y1 = p1.y;
    double x2 = p2.x, y2 = p2.y;
    double x3 = p3.x, y3 = p3.y;

    double a = x1 - x2;
    double b = y1 - y2;
    double c = x1 - x3;
    double d = y1 - y3;

    double e = ((x1 * x1 - x2 * x2) + (y1 * y1 - y2 * y2)) / 2.0;
    double f = ((x1 * x1 - x3 * x3) + (y1 * y1 - y3 * y3)) / 2.0;

    double det = a * d - b * c;
    if (fabs(det) < 1e-8) {
        // 三点共线，无法确定唯一圆，打印log
        printf("[calCircle] Three points are collinear: (%.3f,%.3f), (%.3f,%.3f), (%.3f,%.3f)\n", x1, y1, x2, y2, x3, y3);
        return Circle({0., 0.}, 0.);
    }
    double cx = (e * d - b * f) / det;
    double cy = (a * f - e * c) / det;
    Point center = {cx, cy};
    double radius = calcDis(center, p1);
    return Circle(center, radius);
}

Circle fitCircle(const vector<Point>& points, int iteration = 100, double thres = 0.2) {
    double bestCountNum = 0;
    Circle bestFitCircle;
    for (int i = 0; i < iteration; i++) {
        int idx1 = rand() % points.size();
        int idx2 = rand() % points.size();
        int idx3 = rand() % points.size();
        if (idx1 == idx2 || idx1 == idx3 || idx2 == idx3) continue;
        Circle temFitCircle({0., 0.}, 0.);
    temFitCircle = calCircle(points[idx1], points[idx2], points[idx3]);
    if (temFitCircle.radius == 0.) continue;
        double curCountNum = 0;
        for (int j = 0; j < points.size(); j++) {
            double curDis = calcDis(temFitCircle.center, points[j]);
            if (fabs(curDis - temFitCircle.radius) < thres) {
                curCountNum++;
            }
        }
        if (curCountNum > bestCountNum) {
            bestCountNum = curCountNum;
            bestFitCircle = temFitCircle;
        }
    }

    return bestFitCircle;
}

#include <iostream>
int main() {
    // 构造一组圆上的点（圆心(1,2)，半径3）
    vector<Point> points = {
        {1+3, 2},
        {1-3, 2},
        {1, 2+3},
        {1, 2-3},
        {1+2.12, 2+2.12},
        {1-2.12, 2-2.12},
        {1+2.12, 2-2.12},
        {1-2.12, 2+2.12}
    };

    // 加入一些噪声点
    points.push_back({10, 10});
    points.push_back({-5, 0});
    points.push_back({2, 8});

    Circle result = fitCircle(points, 200, 0.3);
    std::cout << "拟合圆心: (" << result.center.x << ", " << result.center.y << "), 半径: " << result.radius << std::endl;
    return 0;
}
