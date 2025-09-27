#include <stack>

using namespace std;
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int val) : val(val), next(nullptr) {}
};

class MySolution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode* tmp = head;
        if (!tmp) return head;
        stack<ListNode*> node_stack;
        node_stack.push(tmp);
        while (tmp->next) {
            tmp = tmp->next;
            node_stack.push(tmp);
        }
        ListNode* result = node_stack.top();
        ListNode* tmp_ = result;
        while (!node_stack.empty()) {
            tmp_ = node_stack.top();
            tmp_ = tmp_->next;
            node_stack.pop();
        }
        return result;
    }
};

class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode* prev = nullptr;
        ListNode* curr = head;
        while (curr) {
            ListNode* next = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next;
        }
        return prev;
    }
};

class SolutionRecursion {
public:
    ListNode* reverseList(ListNode* head) {
        if (!head || !head->next) return head;
        ListNode* newHead = reverseList(head->next);
        // newHead->next = head;
        head->next->next = head;
        head->next = nullptr;
        return newHead;
    }
};