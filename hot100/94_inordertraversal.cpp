#include <vector>
#include <stack>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(NULL), right(NULL) {}
};

class Solution {
public:
    void traversal(TreeNode* cur, std::vector<int> &vec) {
        if (cur == NULL) return;
        traversal(cur->left, vec);
        vec.push_back(cur->val);
        traversal(cur->right, vec);
    }

    std::vector<int> inordertraversal(TreeNode* root) {
        std::vector<int> result;
        traversal(root, result);
        return result;
    }
};

class iterSolution {
public:
    std::vector<int> inordertraversal(TreeNode* root) {
        std::vector<int> result;
        std::stack<TreeNode*> st;
        TreeNode* cur = root;
        while (cur != NULL || !st.empty()) {
            if (cur != NULL) {
                st.push(cur);
                cur = cur->left;
            }

            else {
                cur = st.top();
                result.push_back(cur->val);
                st.pop();
                cur = cur->right;
            }
        }

        return result;
    }
};