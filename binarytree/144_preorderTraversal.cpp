#include "TreeNode/treenode.h"
#include <vector>

using namespace std;
class Solution {
private:
    void traversal(TreeNode* node, vector<int>& result) {
        if (node == nullptr) return ;
        result.push_back(node->val);
        traversal(node->left, result);
        traversal(node->right, result);
    }
public:
    vector<int> preorderTraversal(TreeNode* root) {
        vector<int> result;
        traversal(root, result);
        return result;
    }
};