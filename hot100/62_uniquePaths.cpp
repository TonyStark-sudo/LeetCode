#include <vector>

class Solution {
public:
    int uniquePath(int m, int n) {
        // 设定size错误，因为dp[0][0]没意义
        // std::vector<std::vector<int>> dp(m, std::vector<int>(n));
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));
        for (int i = 1; i <= m; i++) {
            dp[i][1] = 1;
        }
        for (int i = 1; i <= n; i++) {
            dp[1][i] = 1;
        }

        for (int i = 2; i <= m; i++) {
            for (int j = 2; j <= n; j++) {
                dp[i][j] = dp[i - 1][j] + dp[i][j - 1];
            }
        }

        return dp[m][n];
    }
};