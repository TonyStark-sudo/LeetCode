#include <vector>
#include <iostream>

class Solution {
public:
    int integerBreak(int n) {
        std::vector<int> dp(n + 1);
        // int dp[n + 1] = {0};
        dp[1] = 0;
        dp[2] = 1;
        for (int i = 3; i <= n; i++) {
            for (int j = 1; j < i; j++) {
                dp[i] = std::max(dp[i], std::max(j * (i - j), j * dp[i - j]));
            }
        }
        std::cout << "dp[1] = " << dp[1] << std::endl;
        std::cout << "dp[0] = " << dp[0] << std::endl;
        return dp[n];

    }
};

int main(int argc, char const *argv[])
{
    int a = 101;
    Solution solution;
    int max_value = solution.integerBreak(a);
    std::cout << a << " max value: " << max_value <<std::endl;
    return 0;
}
