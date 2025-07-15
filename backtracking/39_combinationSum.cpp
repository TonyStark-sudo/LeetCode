#include <vector>

class Solution {
private:
    std::vector<int> path;
    std::vector<std::vector<int>> result;
    int sum = 0;
    int target_;
    void backtracking(const std::vector<int>& candidates, int startIndex) {
        if (sum > target_) {
            return ;
        }
        if (sum == target_) {
            result.push_back(path);
            return ;
        }
        for (int i = startIndex; i < candidates.size(); i++) {
            path.push_back(candidates[i]);
            sum += candidates[i];
            backtracking(candidates, i);
            sum -= candidates[i];
            path.pop_back();
        }
    }
public:
    std::vector<std::vector<int>> combinationSum(std::vector<int>& candidates, int target) {
        target_ = target;
        backtracking(candidates, 0);
        return result;
    }
};