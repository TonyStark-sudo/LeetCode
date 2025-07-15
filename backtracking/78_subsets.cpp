#include <vector>

using namespace std;

class Solution {

    vector<vector<int>> result;
    vector<int> subset;
    void backtracking(const vector<int>& nums, int startIndex) {
        for (int i = startIndex; i < nums.size(); i++) {
            subset.push_back(nums[i]);
            result.push_back(subset);
            backtracking(nums, i + 1);
            subset.pop_back();
        }
    }

public:
    vector<vector<int>> subsets(vector<int>& nums) {
        result.push_back({});
        backtracking(nums, 0);
        return result;
    }
};