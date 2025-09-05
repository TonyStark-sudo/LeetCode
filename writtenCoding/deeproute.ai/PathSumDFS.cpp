#include <vector>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(nullptr), right(nullptr) {}
};

class Solution {
private:
    vector<int> path;
    vector<vector<int>> result;
    int sum = 0;
    void dfs(TreeNode* node, int target) {
        if (!node) return ;
        path.push_back(node->val); 
        sum += node->val;
        if (!node->left && !node->right && sum == target) {
            result.push_back(path);
        }
        dfs(node->left, target);
        dfs(node->right, target);
        sum -= node->val;
        path.pop_back();
    }
public:
    bool hasPathSum(TreeNode* root, int target) {
        dfs(root, target);
        if (result.empty()) {
            return false;
        }
        return true;
    }
};