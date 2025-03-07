#include "treenode.h"

class Solution {
public:
    int getDepth(TreeNode* root) {
        if (root == NULL) return 0;
        int leftDepth = getDepth(root->left);
        int rightDepth = getDepth(root->right);
        int depth = 1 + std::max(leftDepth, rightDepth);
        return depth;   
    }

    int maxDepth(TreeNode* root) {
        return getDepth(root);
    }
};

class LevelTraversalSolution {
public:
    int maxDepth(TreeNode* root) {
        if (root == NULL) return 0;
        std::queue<TreeNode*> que;
        que.push(root);
        int depth = 0;
        while (!que.empty()) {
            int size = que.size();
            for (int i = 0; i < size; i++) {
            // for (int i = 0; i < que.size(); i++) { // 错误写法: que在循环过程中会发生变化
                TreeNode* node = que.front();
                que.pop();
                if (node->left) que.push(node->left);
                if (node->right) que.push(node->left);
            }
            depth++;
        }

        return depth;
    }
};

