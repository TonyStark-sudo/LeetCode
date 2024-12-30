
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {};
};

class Solution {
    public:
    // 交换节点的值
    ListNode* swapListVal(ListNode* head) {
        ListNode* dummynode = new ListNode(0);
        dummynode->next = head;
        ListNode* cur = dummynode;
        while (cur->next != nullptr && cur->next->next != nullptr) {
            int temp = cur->next->val;
            cur->next->val = cur->next->next->val;
            cur->next->next->val = temp;
            cur = cur->next->next;
        }
        return head;
    }
    // 交换节点
    ListNode* swapList(ListNode* head) {
        ListNode* dummy_node = new ListNode(0);
        dummy_node->next = head;
        ListNode* cur = dummy_node;
        while (cur->next != nullptr && cur->next->next != nullptr) {
            ListNode* temp1 = cur->next;
            ListNode* temp2 = cur->next->next->next;
            cur->next = cur->next->next;
            cur->next->next = temp1;
            temp1->next = temp2;
            // cur->next->next->next = temp2;
            cur = cur->next->next;
        }
        head = dummy_node->next;
        delete(dummy_node);
        return head;
    } 
};