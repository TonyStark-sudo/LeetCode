#include <vector>

using namespace std;
class Solution {
public:
    int minCostClimbingStairs(vector<int>& cost) {
        // 下标表示在第几层楼梯，dp[i]表示到第i层楼梯的最小花费
        if (cost.size() < 2) return 0;
        vector<int> dp(cost.size() + 1);
        dp[0] = 0;
        dp[1] = 0;
        for (int i = 2; i <= cost.size(); i++) {
            dp[i] = min(dp[i - 1] + cost[i - 1], dp[i - 2] + cost[i - 2]);
        }
        // return min(dp[cost.size() - 1] + cost[cost.size() - 1], dp[cost.size() - 2] + cost[cost.size() - 2]);
        return dp[cost.size()];
    }
};
