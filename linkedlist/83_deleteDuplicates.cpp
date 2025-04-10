// #include "listnode.h"
#include <iostream>

struct ListNode {
    int val;
    ListNode* next;
    // ListNode(int val) : val(val), next(nullptr) {}
    ListNode(int val, ListNode* next = nullptr) : val(val), next(next) {} 
};

class Solution {
public:
    // 面试时所写的
    ListNode* deleteDuplicates(ListNode* head) {
        ListNode* tmp = head;
        // if (head->next == nullptr) return head;
        while (tmp != nullptr && tmp->next != nullptr) {
            if (tmp->val != tmp->next->val) tmp = tmp->next;
            while (tmp != nullptr && tmp->next != nullptr && tmp->val == tmp->next->val) {
                tmp = tmp->next->next;
            }
        }
        return head;
    }

    ListNode* deleteDuplicates2(ListNode* head) {
        ListNode* tmp = head;
        while (tmp != nullptr && tmp->next != nullptr) {
            if (tmp->val == tmp->next->val) {
                tmp->next = tmp->next->next;
            }
            else tmp = tmp->next;
        } 
        return head;
    }

    ListNode* _deleteDuplicates(ListNode* head) {
        if (!head) {
            return head;
        }

        // ListNode* dummy_head = new ListNode(0, head); 
        ListNode* dummy_head = new ListNode(0);
        dummy_head->next = head;
        ListNode* cur = dummy_head;
        while (cur->next && cur->next->next) {
            if (cur->next->val == cur->next->next->val) {
                int x = cur->next->val;
                while (cur->next && cur->next->val == x) {
                    cur->next = cur->next->next;
                }
            }
            else {
                cur = cur->next;
            }
        }

        return dummy_head->next;
    }

};

int main(int argc, char const *argv[])
{
    ListNode* node_0 = new ListNode(0);
    ListNode* node_1 = new ListNode(1);
    ListNode* node_2 = new ListNode(1);
    ListNode* node_3 = new ListNode(3);
    node_0->next = node_1;
    node_1->next = node_2;
    node_2->next = node_3;
    ListNode* dummy_head = node_0;
    // dummy_head->next = node_0;
    while (dummy_head != nullptr) {
        std::cout << dummy_head->val << " ";
        dummy_head = dummy_head->next;
    }
    std::cout << std::endl;
    Solution solution;
    ListNode* res = solution._deleteDuplicates(node_0);

    while (res != nullptr) {
        std::cout << res->val << " ";
        res = res->next;
    }
    return 0;
}
