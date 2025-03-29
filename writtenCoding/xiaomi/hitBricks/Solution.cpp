#include <vector>
#include <iostream>

void creatNums(std::vector<int>& nums) {
    for (int i = 0; i < nums.size(); i++) {  
        nums[i] = std::rand() % 3 + 1;
    }

    for (int& _ : nums) {
        std::cout << _ << " ";
    }
}

class Solution {
public:
    Solution(int _a, int _b, int _c, int _n) : a(_a), b(_b), c(_c), n(_n) {}                                                                    


private:
    int a, b, c, n;
};

int main(int argc, char *argv[])
{
    int n, a, b, c;
    std::cin >> n >> a >> b >> c;
    std::cout << n << " " << a << " " << b << " " << c << std::endl;
    std::vector<int> nums(n);

    for (int i = 0; i < nums.size(); i++) {
        std::cin >> nums[i]; 
    }
    std::vector<std::vector<int>> dp{n, std::vector<int>(n, 0)};
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    for (int i = n - 1; i >= 0; --i) {
        dp[i][i] = a; 
        for (int j = i + 1; j < n; j++) {
            int r = std::max(dp[i + 1][j], dp[i][j - 1]) + a;
            for (int k = i + 1; k < j; ++k) {
                r = std::max(r, dp[i][k] + dp[k + 1][j]);
            }
            if (nums[j] == nums[i]) {
                r = std::max(r, dp[i + 1][j - 1] + b);
                for (int k = i + 1; k < j; ++k) {
                    if (nums[i] == nums[k]) {
                        r = std::max(r, dp[i + 1][k - 1] + dp[k + 1][j - 1] + c);
                    }
                }
            }
            dp[i][j] = r;
        }
    }
    std::cout << dp[0][n - 1] << std::endl;
    return 0;
}
