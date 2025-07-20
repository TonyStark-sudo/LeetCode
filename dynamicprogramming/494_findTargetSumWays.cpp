#include <vector>
using namespace std;
class Solution_use_dfs {
private:
    int sum = 0;
    int select_sum = 0;
    int method_num = 0;
    int target_;
    // 这个递归写法不全
    void backtracking(const vector<int>& nums, int startIndex) {
        if (startIndex >= nums.size() || select_sum - (sum - select_sum) >= target_) {
            if (select_sum - (sum - select_sum) == target_) {
                method_num++;
            }
            if (nums[startIndex] != 0) return ;
        }
        for (int i = startIndex; i < nums.size(); i++) {
            select_sum += nums[i];
            backtracking(nums, i + 1);
            select_sum -= nums[i];
        }
    }
    // 这个写法不超时，能过
    void backtracking(const vector<int>& nums, int index, int current_sum) {
        if (index == nums.size()) {
            if (current_sum == target_) method_num++;
            return;
        }
        backtracking(nums, index + 1, current_sum + nums[index]);
        backtracking(nums, index + 1, current_sum - nums[index]);
    }

public:
    int findTargetSumWays(vector<int>& nums, int target) {
        target_ = target;
        for (int i= 0; i < nums.size(); i++) {
            sum += nums[i];
        }
        backtracking(nums, 0);
        return method_num;
    }
};

class Solution_use_dp2dim {
public:
    int findTargetSumWays(vector<int>& nums, int target) {
        int sum = 0;
        for (int i = 0; i < nums.size(); i++) sum += nums[i];
        if (abs(target) > sum) return 0; // 此时没有方案
        if ((target + sum) % 2 == 1) return 0; // 此时没有方案
        int bagSize = (target + sum) / 2;
        
        vector<vector<int>> dp(nums.size(), vector<int>(bagSize + 1, 0));
        
        // 初始化最上行
        if (nums[0] <= bagSize) dp[0][nums[0]] = 1; 

        // 初始化最左列，最左列其他数值在递推公式中就完成了赋值
        dp[0][0] = 1; 

        int numZero = 0;
        for (int i = 0; i < nums.size(); i++) {
            if (nums[i] == 0) numZero++;
            dp[i][0] = (int) pow(2.0, numZero);
        }

        // 以下遍历顺序行列可以颠倒
        for (int i = 1; i < nums.size(); i++) { // 行，遍历物品
            for (int j = 0; j <= bagSize; j++) { // 列，遍历背包
                if (nums[i] > j) dp[i][j] = dp[i - 1][j]; 
                else dp[i][j] = dp[i - 1][j] + dp[i - 1][j - nums[i]];
            }
        }
        return dp[nums.size() - 1][bagSize];
    }
};

class Solution_use_dp1dim {
public:
    int findTargetSumWays(vector<int>& nums, int target) {
        int sum = 0;
        for (int i = 0; i < nums.size(); i++) sum += nums[i];
        if (abs(target) > sum) return 0;
        int bagSize = (target + sum) / 2;
        if ((target + sum) % 2) return 0;
        vector<int> dp(bagSize + 1, 0);
        dp[0] = 1;
        for (int i = 0; i < nums.size(); i++) {
            for (int j = bagSize; j >= nums[i]; j--) {
                dp[j] += dp[j - nums[i]];
            }
        }
        return dp[bagSize];
    }
};