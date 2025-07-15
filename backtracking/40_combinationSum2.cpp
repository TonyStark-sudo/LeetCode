#include <vector>

class Solution {
private:
    std::vector<int> path;
    std::vector<std::vector<int>> result;
    int sum = 0;
    int target_;
    void backtracking(const std::vector<int>& candidates, int startIndex, std::vector<int>& used) {
        if (sum > target_) {
            return ;
        }
        if (sum == target_) {
            result.push_back(path);
            return ;
        }
        for (int i = startIndex; i < candidates.size(); i++) {
            if (i > 0 && candidates[i] == candidates[i - 1] && !used[i - 1]) {
                continue;
            }
            path.push_back(candidates[i]);
            used[i] = 1;
            sum += candidates[i];
            backtracking(candidates, i + 1, used);
            sum -= candidates[i];
            used[i] = 0;
            path.pop_back();
        }
    }
public:
    std::vector<std::vector<int>> combinationSum2(std::vector<int>& candidates, int target) {
        target_ = target;
        std::sort(candidates.begin(), candidates.end());
        std::vector<int> used(candidates.size(), 0);
        backtracking(candidates, 0, used);
        return result;
    }
};