#include <vector>
// #include "treenode.h"
#include <iostream>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(nullptr), right(nullptr) {}
};

// 只能通过后续遍历释放堆资源
void delete_postorder(TreeNode* node) {
    if (!node) return ;
    delete node;
    delete_postorder(node->left);
    delete_postorder(node->right);
}

using namespace std;

void traversal(TreeNode* node) {
    if (node == nullptr) return ;
    traversal(node->left);
    traversal(node->right);
    std::cout << node->val << " ";
}

class Solution {
private:
    void traversal(TreeNode* node, vector<int>& result) {
        if (node == nullptr) return ;
        traversal(node->left, result);
        traversal(node->right, result);
        // result.push_back(node->val);
        std::cout << node->val << " ";
    }
public:
    vector<int> postorderTraversal(TreeNode* root) {
        vector<int> result;
        traversal(root, result);
        return result;
    }
};

int main(int argc, char const *argv[])
{
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    root->right->left = new TreeNode(6);
    root->right->right = new TreeNode(7);

    traversal(root);
    delete_postorder(root);
    return 0;
}
