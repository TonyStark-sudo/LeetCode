#include <vector>

using namespace std;

class Solution {
    vector<vector<int>> subsets;
    vector<int> subset;
    void backtracking(const vector<int>& nums, int startIndex, vector<int>& used) {
        subsets.push_back(subset);
        for (int i = startIndex; i < nums.size(); i++) {
            if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1]) {
                continue;
            }
            subset.push_back(nums[i]);
            used[i] = 1;
            backtracking(nums, i + 1, used);
            used[i] = 0;
            subset.pop_back();
        }
    }

public:
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        vector<int> used(nums.size(), 0);
        sort(nums.begin(), nums.end());
        backtracking(nums, 0, used);
        return subsets;
    }
};