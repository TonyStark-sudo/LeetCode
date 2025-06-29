#include <algorithm>
#include <vector>

using namespace std;

bool cmp(const vector<int>& boon1, const vector<int>& boon2) {
    return boon1[0] < boon2[0];
}
class Solution {
private:
    bool hasCommon(const vector<int>& interval1, const vector<int>& interval2) {
        if (interval1[0] > interval2[1] || interval1[1] < interval2[0]) {
            return false;
        }
        return true;
    }
public:
    int findMinArrowShots(vector<vector<int>>& points) {
        if (points.size() == 0) return 0;
        std::sort(points.begin(), points.end(), cmp);
        int arrow_num = 1;
        for (int i = 1; i < points.size(); i++) {
            if (points[i][0] > points[i - 1][1]) {
                arrow_num++;
            }
            else {
                points[i][1] = min(points[i - 1][1], points[i][1]);
            }
        }
        return arrow_num;
    }
};
