#include <vector>
#include <unordered_set>

class Solution {
private:
    std::vector<std::vector<int>> results;
    std::vector<int> path;
    void backtracking(std::vector<int>& nums, int startIndex) {
        if (path.size() > 1) {
            results.push_back(path);
        }
        std::unordered_set<int> uset;
        for (int i = startIndex; i < nums.size(); i++) {
            if (!path.empty() && nums[i] < path.back() 
                || uset.find(nums[i]) != uset.end()) {
                    continue;
                }
            uset.insert(nums[i]);
            path.push_back(nums[i]);
            backtracking(nums, i + 1);
            path.pop_back();
        }
    }
public:
    std::vector<std::vector<int>> findSubsequences(std::vector<int>& nums) {
        backtracking(nums, 0);
        return results;
    }
};