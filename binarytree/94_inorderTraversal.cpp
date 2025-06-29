#include <treenode.h>
#include <vector>

using namespace std;

class Solution {
private:
    void traversal(TreeNode* node, vector<int>& result) {
        if (node == nullptr) return ;
        traversal(node->left, result);
        result.push_back(node->val);
        traversal(node->right, result);
    }
public:
    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> result;
        traversal(root, result);
        return result;
    }
};