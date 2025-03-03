/*
二叉树的递归遍历：
1、确定递归函数的参数和返回值
2、确定终止条件
3、确定单层递归的逻辑
*/

#include <vector>
#include <iostream>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(nullptr), right(nullptr) {}
};

// 前序遍历 中左右
void preorderTraversal(TreeNode* cur, std::vector<int>& vec) {
    if (cur == NULL) return;
    vec.push_back(cur->val);
    preorderTraversal(cur->left, vec);
    preorderTraversal(cur->right, vec);
}

// 中序遍历 左中右
void inorderTraversal(TreeNode* cur, std::vector<int>& vec) {
    if (cur == NULL) return;
    inorderTraversal(cur->left, vec);
    vec.push_back(cur->val);
    inorderTraversal(cur->right, vec);
}

// 后序遍历 左右中
void postorderTraversal(TreeNode* cur, std::vector<int>& vec) {
    if (cur == NULL) return;
    postorderTraversal(cur->left, vec);
    postorderTraversal(cur->right, vec);
    vec.push_back(cur->val);
}

class Solution {
public:
    std::vector<int> Traversal(TreeNode* root) {
        std::vector<int> result;
        preorderTraversal(root, result);
        return result;
    }
};

int main(int argc, char *argv[])
{
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    root->right->right = new TreeNode(6);

    std::cout << "preordertraversal: " << std::endl;
    std::vector<int> preresults;
    preorderTraversal(root, preresults);
    for (auto result : preresults) {
        std::cout << result << " ";
    }

    std::cout << "\ninordertraversal: " << std::endl;
    std::vector<int> inresults;
    inorderTraversal(root, inresults);
    for (auto result : inresults) {
        std::cout << result << " ";
    }

    std::cout << "\npostordertraversal: " << std::endl;
    std::vector<int> postresults;
    postorderTraversal(root, postresults);
    for (auto result : postresults) {
        std::cout << result << " ";
    }

    /* code */
    return 0;
}
