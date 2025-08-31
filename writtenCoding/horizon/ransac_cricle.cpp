#include <vector>

using namespace std;

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

double calcDis(const Point& p1, const Point& p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

struct Cricle {
    Point center;
    double radius;
    Cricle(Point c, double r) : center(c), radius(r) {}
};

Cricle fitCricle (const vector<Point>& points) {

}

Cricle ransacCricle(const vector<Point> points, double thres, int iterNum) {
    int bestCountNum = 0;
    Cricle cricle(Point{0, 0}, 0.0);

    for (int i = 0; i < iterNum; i++) {
        vector<Point> targetPoints;
        int idx1 = rand() % points.size();
        int idx2 = rand() % points.size();
        int idx3 = rand() % points.size();
        if (idx1 == idx2 || idx2 == idx3 || idx1 == idx3) continue;
        targetPoints.push_back(points[idx1]);
        targetPoints.push_back(points[idx2]);
        targetPoints.push_back(points[idx3]);
        Cricle fitedCricle = fitCricle(targetPoints);
        int countNum = 0;
        for (Point point : points) {
            double dis = calcDis(fitedCricle.center, point);
            if (dis <= fitedCricle.radius + thres) {
                countNum++;
            }
        }
        if (countNum > bestCountNum) {
            bestCountNum = countNum;
            cricle = fitedCricle; 
        }
    }
    return cricle;
}
