#include <vector>
#include <queue>
#include <stack>
#include <iostream>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int value) : val(value), left(NULL), right(NULL) {}
};

class Solution {
public:
    // not good version
    static std::vector<int> levelOrder(TreeNode* root) {
        std::vector<int> result;
        // std::stack<TreeNode*> st;
        std::queue<TreeNode*> que;
        result.push_back(root->val);
        que.push(root);
        while (!que.empty()) {
            // result.push_back(st.top()->val);
            TreeNode* node = que.front();
            que.pop();
            if (node->left != NULL && node->right != NULL) {
                result.push_back(node->left->val);
                result.push_back(node->right->val);
            }
            
            if (node->left) que.push(node->left);
            if (node->right) que.push(node->right);
        }
        
        return result;
    }

    std::vector<std::vector<int>> levelOrder2(TreeNode* root) {
        std::queue<TreeNode*> que;
        if (root != NULL) que.push(root);
        std::vector<std::vector<int>> result;
        while (!que.empty()) {
            int size = que.size();
            std::vector<int> vec;

            for (int i = 0; i < size; i++) {
                TreeNode* node = que.front();
                que.pop();
                vec.push_back(node->val);
                if (node->left) que.push(node->left);
                if (node->right) que.push(node->right);
            }

            result.push_back(vec);
        }

        return result;
    }
};

int main(int argc, char *argv[])
{
    TreeNode* root = new TreeNode(5);
    root->left = new TreeNode(4);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(2);
    root->left->right = new TreeNode(1);
    std::vector<int> result = Solution::levelOrder(root);
    
    for (auto _ : result) {
        std::cout << _ << " ";
    }

    /* code */
    return 0;
}
