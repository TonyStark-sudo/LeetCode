
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {};
}; 

class Solution {
    public:
    ListNode* detectCycle(ListNode* head) {

        ListNode* fast = head;
        ListNode* slow = head;
        while (fast != nullptr && fast->next != nullptr) {
            fast = fast->next->next;
            slow = slow->next;
            if (fast == slow) {
                ListNode* index_1 = fast;
                ListNode* index_2 = head;
                while (index_1 != index_2) {
                    index_1 = index_1->next;
                    index_2 = index_2->next;
                }

                return index_1;
            }
        }

        return nullptr;

    }
};