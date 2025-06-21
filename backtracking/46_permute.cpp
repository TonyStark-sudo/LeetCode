#include <vector>

class Solution {
private:
    std::vector<std::vector<int>> result;
    std::vector<int> path;
    void backtracking(std::vector<int>& nums, std::vector<bool>& used) {
        if (path.size() == nums.size()) {
            result.push_back(path);
            return ;
        }
        for (int i = 0; i < nums.size(); i++) {
            // if (!used[i]) {
            //     path.push_back(nums[i]);
            // }
            if (used[i]) continue;
            path.push_back(nums[i]);
            used[i] = true;
            backtracking(nums, used);
            used[i] = false;
            path.pop_back();
        }
    }
public:
    std::vector<std::vector<int>> permute(std::vector<int>& nums) {
        std::vector<bool> used(nums.size(), false);
        backtracking(nums, used);
        return result;
    }
};