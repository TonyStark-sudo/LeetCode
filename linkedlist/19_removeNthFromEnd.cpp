
struct ListNode
{
    /* data */
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {};
};

class Solution {
    public:
    // 求出链表size,倒数第n个就是正数（size - n）
    ListNode* removeNthFromEnd(ListNode* head, int n) {

        int size = 0;
        ListNode* dummy_node = new ListNode(0);
        dummy_node->next = head;
        ListNode* cur = dummy_node;
        while (cur->next != nullptr) {
            cur = cur->next;
            size++;
        }
        int cnt = size - n;
        cur = dummy_node;
        while (cnt--) {
            cur = cur->next;    
        }
        ListNode* temp = cur->next;
        cur->next = cur->next->next;
        delete(temp);
        head = dummy_node->next;
        delete(dummy_node);
        return head;

    }
};