#include <vector>
#include <stack>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int value) : val(value), left(nullptr), right(nullptr) {} 
};

std::vector<int> inorderTraversal(TreeNode* root) {
    std::vector<int> result;
    std::stack<TreeNode*> st;
    if (root == NULL) return {};
    if (root != NULL) st.push(root);
    while (!st.empty()) {
        TreeNode* node = st.top();
        if (node != NULL) {
            st.pop();
            if (node->right) st.push(node->right);
            st.push(node);
            st.push(NULL);
            if (node->left) st.push(node->left);
        }

        else {
            st.pop();
            node = st.top();
            st.pop();
            result.push_back(node->val);
        }
    }

    return result;
}

std::vector<int> preorderTraversal(TreeNode* root) {
    std::vector<int> result;
    std::stack<TreeNode*> st;
    if (root == NULL) return {};
    if (root != NULL) st.push(root);
    while (!st.empty()) {
        TreeNode* node = st.top();
        if (node != NULL) {
            st.pop();

            if (node->right) st.push(node->right);
            if (node->left) st.push(node->left);
            st.push(node);
            st.push(NULL);
        }

        else {
            st.pop();
            node = st.top();
            st.pop();
            result.push_back(node->val);
        }
    }

    return result;
}

std::vector<int> postorderTraversal(TreeNode* root) {
    std::vector<int> result;
    std::stack<TreeNode*> st;
    if (root == NULL) return {};
    if (root != NULL) st.push(root);
    while (!st.empty()) {
        TreeNode* node = st.top();
        if (node != NULL) {
            st.pop();

            st.push(node);
            st.push(NULL);
            if (node->right) st.push(node->right);
            if (node->left) st.push(node->left);
        }

        else {
            st.pop();
            node = st.top();
            st.pop();
            result.push_back(node->val);
        }
    }

    return result;
}