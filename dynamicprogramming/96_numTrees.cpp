#include <vector>

// 当N为节点个数时，
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    int numTrees(int n) {
        // std::vector<std::vector<int>> dp()
        if (n <= 2) return n;
        std::vector<int> dp(n + 1);
        dp[0] = 1;
        dp[1] = 1;
        // dp[2] = 2;
        for (int i = 2; i <= n; i++) {
            for (int j = 0; j < i; j++) {
                dp[i] += dp[j] * dp[i - (j + 1)];
            }
        }
        return dp[n];
    }
};