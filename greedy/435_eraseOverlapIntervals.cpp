#include <vector>
#include <algorithm>
using namespace std;

bool cmp(const vector<int>& internal1, const vector<int>& internal2) {
    if (internal1[0] < internal2[0]) return true;
    return internal1[0] < internal2[0];
}

class Solution {
public:
    int eraseOverlapIntervals(vector<vector<int>>& intervals) {
        int result = 0;
        sort(intervals.begin(), intervals.end(), cmp);
        for (int i = 1; i < intervals.size(); i++) {
            if (intervals[i][0] < intervals[i - 1][1]) {
                intervals[i][1] = intervals[i - 1][1];
                intervals[i][1] = min(intervals[i - 1][1], intervals[i][1]);
                result++;
            }
        }
        return result;
    }
};