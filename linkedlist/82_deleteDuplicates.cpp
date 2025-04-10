
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int val) : val(val), next(nullptr) {}
    ListNode(int val, ListNode* next = nullptr) : val(val), next(next) {}
};

class Solution {
public:
    ListNode* deleteDuplicates(ListNode* head) {

        ListNode* dummy_head = new ListNode(0, head);
        ListNode* cur = dummy_head;
        while (cur && cur->next && cur->next->next) {
            if (cur->next->val == cur->next->next->val) {
                cur->next = cur->next->next->next;
            }
            else {
                cur->next = cur->next->next;
            }
        }

        return dummy_head->next;
    }

    ListNode* deleteDuplicates2(ListNode* head) {
        ListNode* dummy_head = new ListNode(0, head);
        ListNode* cur = dummy_head;
        while (cur && cur->next && cur->next->next) {
            if (cur->next->val == cur->next->next->val) {
                int val = cur->next->val;
                while (cur->next && cur->next->val == val) {
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