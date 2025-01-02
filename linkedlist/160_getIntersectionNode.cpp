
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

class Solution {
    public:
    ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {

        while (headA != nullptr) {
            ListNode* cur = headB;
            while (cur != nullptr) {
                if (headA == cur) {
                    return cur;
                }
                cur = cur->next;
            }
            headA = headA->next;
        }

        return nullptr;

    }

};

