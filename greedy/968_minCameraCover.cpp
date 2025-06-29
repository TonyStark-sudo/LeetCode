#include "treenode.h"

using namespace std;

class Solution {
private:
    int result = 0;
    int traversal(TreeNode* node) {
        if (!node) return 2;
        int left = traversal(node->left);
        int right = traversal(node->right);
        if (left == 2 && right == 2) return 0;
        else if (left == 0 || right == 0) {
            result++;
            return 1;
        }
        else if (left == 1 || right == 1) return 2;
        return -1;
    }
public:
    int minCameraCover(TreeNode* root) {
        if (traversal(root) == 0) {
            result++;
        }
        return result;
    }
};