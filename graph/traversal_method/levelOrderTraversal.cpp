#include <vector>
#include <iostream>
#include <queue>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    std::vector<int> levelOrderTraversal(TreeNode* root) {
        std::vector<int> path;
        std::queue<TreeNode*> level_que;
        level_que.push(root);
        while(!level_que.empty()) {
            int size = level_que.size();
            for (int i = 0; i < size; i++) {
                TreeNode* cur = level_que.front();
                level_que.pop();
                path.push_back(cur->val);
                if (cur->left) level_que.push(cur->left);
                if (cur->right) level_que.push(cur->right);
            }
        }
        return path;
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
    Solution solution;
    std::vector<int> path = solution.levelOrderTraversal(root);
    for (int& ele : path) {
        std::cout << ele << " ";
    }
    std::cout << std::endl;
    delete root->right->right;
    delete root->right->left;
    delete root->left->right;
    delete root->left->left;
    delete root->right;
    delete root->left;
    delete root;
    return 0;
}
