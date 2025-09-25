#include <iostream>
#include <vector>

using namespace std;
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int value) : val(value), left(nullptr), right(nullptr) {} 
};

class MySolution {
private:
    void PreOrderTraversal(TreeNode* node, vector<TreeNode*>& result) {
        if (!node) return ;
        result.push_back(node);
        PreOrderTraversal(node->left, result);
        PreOrderTraversal(node->right, result);
    }
public:
    void flatten(TreeNode* root) {
        TreeNode* tmp = root;
        vector<TreeNode*> l;
        PreOrderTraversal(tmp, l);
        for (int i = 1; i < l.size(); i++) {
            TreeNode* prev = l[i - 1], * cur = l[i];
            prev->left = nullptr;
            prev->right = cur;
        }
    }
};

class Solution {
public:
    void flatten(TreeNode* root) {
        TreeNode* curr = root;
        while (curr) {
            if (curr->left) {
                // 找到左子树最右节点
                TreeNode* pre = curr->left;
                while (pre->right) {
                    pre = pre->right;
                }
                // 将右子树接到左子树最右节点
                pre->right = curr->right;
                // 左子树变为右子树
                curr->right = curr->left;
                curr->left = nullptr;
            }
            curr = curr->right;
        }
    }
};