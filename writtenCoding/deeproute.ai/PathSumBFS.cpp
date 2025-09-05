#include <vector>
#include <queue>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    bool hasPathSum(TreeNode* root, int target) {
        queue<int> val_que;
        queue<TreeNode*> node_que;
        if (!root) return false;
        val_que.push(root->val);
        node_que.push(root);
        while (!node_que.empty()) {
            TreeNode* cur = node_que.front();
            int temp = val_que.front();
            node_que.pop();
            val_que.pop();
            if (!cur->left && !cur->right) {
                if (temp == target) {
                    return true;
                }
                continue;
            }
            if (cur->left) {
                node_que.push(cur->left);
                val_que.push(cur->left->val + temp);
            }
            if (cur->right) {
                node_que.push(cur->right);
                val_que.push(cur->right->val + temp);
            }
        }
        return false;
    }
};