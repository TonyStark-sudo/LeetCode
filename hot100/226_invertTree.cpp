#include <queue>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {

        if (root == nullptr) return root;
        std::queue<TreeNode*> que;
        que.push(root);
        while (!que.empty()) {
            TreeNode* node;
            node = que.front();
            que.pop();
            std::swap(node->left, node->right);
            // TreeNode* node, *tmp;
            // node = que.front();
            // que.pop();
            // tmp = node->left;
            // node->left = node->right;
            // node->right = tmp;
            if (node->left) que.push(node->left);
            if (node->right) que.push(node->right);
        }

        return root;
    }
};