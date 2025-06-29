#include <algorithm>
#include <vector>

using namespace std;

bool cmp(const vector<int>& interval1, const vector<int>& internal2) {
    return interval1[0] < internal2[0];
}
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        vector<vector<int>> merged;
        if (intervals.size() == 1) merged = intervals;
        sort(intervals.begin(), intervals.end(), cmp);
        for (int i = 1; i < intervals.size(); i++) {
            if (intervals[i][0] > intervals[i - 1][1]) {
                merged.push_back(intervals[i - 1]);
            }
            else {
                intervals[i][1] = max(intervals[i][1], intervals[i - 1][1]);
                // intervals[i][1] = intervals[i][1];
                intervals[i][0] = min(intervals[i][0], intervals[i - 1][0]);
                // intervals[i][0] = intervals[i - 1][0];
            }
            if (i == intervals.size() - 1) merged.push_back(intervals[i]);
        }
        return merged;
    }
};