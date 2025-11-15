/*
题目二：
给你一组二维平面上的点云，用ransac的思想将它们拟合成一条直线
{1, 0, },
{2, 1, },
{3, 2, },
{4, 3, },
{5, 4, },
{6, 5, },
{7, 6, },
{8, 7, },
{8, 0, },
{1, 7, },

已知直线上的两点P1(X1,Y1) P2(X2,Y2)， P1 P2两点不重合。则直线的一般式方程AX+BY+C=0中，A B C分别等于：
A = Y2 - Y1
B = X1 - X2
C = X2*Y1 - X1*Y2

点到直线的距离公式: |Ax+By+C|/sqrt(A^2+B^2)
*/
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <cmath>
#include <iostream>

using namespace std;
struct Point
{
    double x;
    double y;
};

void calcLineParams(const Point &p1, const Point &p2, double& A, double& B, double& C)
{
    A = p2.y - p1.y;
    B = p1.x - p2.x;
    C = p2.x * p1.y - p1.x * p2.y;
}

inline double calcDis(const Point& p, const double& A, const double& B, const double& C) {

    // return std::abs(A * p.x + B * p.y + C) / (A * A + B * B);
    return (A * p.x + B * p.y + C) * (A * p.x + B * p.y + C) / (A * A + B * B);
}

int main(int argc, char const *argv[])
{
    vector<Point> points = {
        {1, 0, },
        {2, 1, },
        {3, 2, },
        {4, 3, },
        {5, 4, },
        {6, 5, },
        {7, 6, },
        {8, 7, },
        {8, 0, },
        {1, 7, },
    };
    double min_sum_dis = 100;
    double b_A, b_B, b_C;
    for (size_t i{0UL}; i < 1000; i++) {
        int idx1 = rand() % points.size();
        int idx2 = rand() % points.size();
        while (idx1 == idx2) {
            idx1 = rand() % points.size();
            idx1 = rand() % points.size();
        }
        double A, B, C;
        calcLineParams(points[idx1], points[idx2], A, B, C);
        double sum_dis = 0;
        for (size_t j{0UL}; j < points.size(); j++) {
            double dis_tmp = calcDis(points[j], A, B, C);
            sum_dis += dis_tmp;
        }
        if (sum_dis < min_sum_dis) {
            min_sum_dis = sum_dis;
            b_A = A;
            b_B = B;
            b_C = C;
        }
    }
    cout << "Best A B C: " << b_A << " " <<  b_B  << " " << b_C << endl;
    return 0;
}