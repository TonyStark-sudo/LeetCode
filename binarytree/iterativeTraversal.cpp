#include <vector>
#include <stack>
#include <algorithm>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(NULL), right(NULL) {}
};

std::vector<int> preorderTraversal(TreeNode* root) {
    std::vector<int> result;
    std::stack<TreeNode*> st;
    if (root == NULL) return result;
    st.push(root);
    while (!st.empty()) {
        TreeNode* temp = st.top();
        st.pop();
        result.push_back(temp->val);
        if (temp->right != NULL) st.push(temp->right);
        if (temp->left != NULL) st.push(temp->left);
    } 
    return result;
}

std::vector<int> inorderTraversal(TreeNode* root) {
    std::stack<TreeNode*> st;
    std::vector<int> result;
    if (root == NULL) return result;

    TreeNode* cur = root;
    while (cur != NULL || !st.empty()) {
        if (cur != NULL) {
            st.push(cur);
            cur = cur->left;
        }
        else {
            cur = st.top();
            st.pop();
            result.push_back(cur->val);
            cur = cur->right;
        }

    }
    return result; 
}

std::vector<int> postorderTraversal(TreeNode* root) {
    std::vector<int> result;
    std::stack<TreeNode*> st;
    if (root == NULL) return {};
    st.push(root);
    while (!st.empty()) {
        TreeNode* node = st.top();
        st.pop();
        result.push_back(node->val);

        if (node->left != nullptr) st.push(node->left);
        if (node->right != nullptr) st.push(node->right);
    }
    reverse(result.begin(), result.end());
    return result;
}