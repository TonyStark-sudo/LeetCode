#include "treenode.h"

class Solution {
public:
    // 什么是二叉搜索树
    // 左子树所有节点值小于根节点，右子树所有节点值大于根节点
    int numTrees(int n) {
        std::vector<int> dp(n + 1);
        dp[0] = 1;
        // 遍历n个元素组成的二叉搜索树的n个元素,控制维度，自然是从小到大遍历
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= i; j++) {
                // 控制加数的个数，每个加数两个因数的下标有规律
                dp[i] += dp[j - 1] * dp[i - j];
            }
        }     
        return dp[n];
    }
};
