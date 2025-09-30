#include <set>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
private:
    bool cmp(const vector<int>& vec1, const vector<int>& vec2) {
        if (vec1[0] == vec2[0]) {
            return vec1[1] < vec2[1];
        }
        return vec1[0] < vec2[0];
    }
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        vector<vector<int>> intervals_merged;
        sort(intervals.begin(), intervals.end(), cmp);
        for (int i = 1; i < intervals.size(); i++) {
            if (intervals[i - 1].at(1) < intervals[i].at(0)) {
                intervals_merged.push_back(intervals[i - 1]);
            }
            else {
                intervals[i][0] = min(intervals[i][0], intervals[i - 1][0]);
                intervals[i][1] = max(intervals[i][1], intervals[i - 1][1]);
            }
            // if (intervals[i - 1].at(1) >= intervals[i].at(0)) {
            //     intervals[i - 1].at(1) = intervals[i].at(1);
            //     intervals[i] = intervals[i - 1];
            // }
            if (i == intervals.size() - 1) intervals_merged.push_back(intervals[i]);
        }
        return intervals_merged;
        // set<vector<int>> merged_set(intervals.begin(), intervals.end());
    }
};