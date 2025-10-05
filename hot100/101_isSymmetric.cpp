#include <stack>
#include <vector>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : val(val), left(nullptr), right(nullptr) {}
};

void trversalLeft(TreeNode* root, std::vector<int>& vec) {
    if (root == NULL) return;
    trversalLeft(root->left, vec);
    vec.push_back(root->val);
    trversalLeft(root->right, vec);
}
void traversalRight(TreeNode* root, std::vector<int>& vec) {
    if (root == NULL) return;
    traversalRight(root->right, vec);
    vec.push_back(root->val);
    traversalRight(root->left, vec);
}


class MySolution {
public:
    void traversalLeft(TreeNode* root, std::vector<int>& vec) {
        if (root == NULL) return;
        traversalLeft(root->left, vec);
        vec.push_back(root->val);
        traversalLeft(root->right, vec);
    }
    void traversalRight(TreeNode* root, std::vector<int>& vec) {
        if (root == NULL) return;
        traversalRight(root->right, vec);
        vec.push_back(root->val);
        traversalRight(root->left, vec);
    }

    bool isSymmetric(TreeNode* root) {
        TreeNode* leftTree = root->left;
        TreeNode* rightTree = root->right;
        std::vector<int> vec_left;
        std::vector<int> vec_right;
        traversalLeft(leftTree, vec_left);
        traversalRight(rightTree, vec_right);
        if (vec_left == vec_right) return true;
        else return false;
        
    }

    bool _isSymmetric(TreeNode* root) { 
        TreeNode* leftTree = root->left;
        TreeNode* rightTree = root->right;
        std::stack<TreeNode*> st_left;
        st_left.push(leftTree);
        std::stack<TreeNode*> st_right;
        st_right.push(rightTree);

        while (!st_left.empty() && !st_right.empty()) {
            if (leftTree != nullptr || leftTree != nullptr) {                
                st_left.push(leftTree);
                leftTree = leftTree->left;
                st_right.push(rightTree);
                rightTree = rightTree->right;
            }

            else {
                if (st_left.top()->val != st_right.top()->val) return false;
                st_left.pop();
                st_right.pop();
            }
        }

    }
};

class RecursionSolution {
public:
    bool compare(TreeNode* left, TreeNode* right) {
        if (left == NULL && right != NULL) return false;
        else if (left != NULL && right == NULL) return false;
        else if (left == NULL && right == NULL) return true;
        else if (left->val != right->val) return false;

        bool outside = compare(left->left, right->right);
        bool inside = compare(left->right, left->left);
        bool isSame = outside && inside;
        return isSame; 
    }

    bool isSymmetric(TreeNode* root) {
        if (root == NULL) return true;
        return compare(root->left, root->right);

    }
};

class Solution {
private:
    bool check(TreeNode* p, TreeNode* q) {
        if (!q && !p) return true;
        if (!q || !p) return false;
        if (q->val != p->val) return false;
        return check(p->left, q->right) && check(q->right, p->left);
    }
public:
    bool isSymmetric(TreeNode* root) {
        return check(root->left, root->right);
    }
};