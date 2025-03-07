#include "treenode.h"

class Solution {
public:
    int getminDepth(TreeNode* root) {
        if (root == NULL) return 0;
        int leftdepth = getminDepth(root->left);
        int rightdepth = getminDepth(root->right);
        // int mindepth = (1 + std::min(leftdepth, rightdepth)); // 错误写法如果左子树为空，右子树不为空的话，
                                                                 // 则最小深度是右子树深度+1
        if (root->left == NULL && root->right != NULL) {
            return rightdepth + 1;
        }

        if (root->right == NULL && root->left != NULL) {
            return leftdepth + 1;
        }

        int result = 1 + std::min(leftdepth, rightdepth);
    }

    int minDepth(TreeNode* root) {
        getminDepth(root);
    }
};