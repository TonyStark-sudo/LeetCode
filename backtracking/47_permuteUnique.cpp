#include <vector>
#include <algorithm>

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
            if (used[i] || i > 0 && nums[i] == nums[i - 1] && used[i - 1] == false) {
                continue;
            }
            used[i] = true;
            path.push_back(nums[i]);
            backtracking(nums, used);
            path.pop_back();
            used[i] = false;
        }    
    }
public:
    std::vector<std::vector<int>> permuteUnique(std::vector<int>& nums) {
        std::vector<bool> used(nums.size(), false);
        std::sort(nums.begin(), nums.end());      
        backtracking(nums, used);
        return result;
    }
};