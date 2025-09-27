#include <unordered_set>

struct ListNode
{
    int val;
    ListNode* next;
    ListNode(int val) : val(val), next(nullptr) {}
};


class Solution {
public:
    bool hasCycle(ListNode* head) {
        int n = 10001;
        ListNode* tmp = head;
        while (n--) {
            tmp = tmp->next;
            if (!tmp->next) return false;
        }
        return true;
    }
};

class MySolution {
public:
    bool hasCycle(ListNode* head) {
        std::unordered_set<ListNode*> seen;
        while (!head) {
            if (seen.count(head)) {
                return true;
            }
            seen.insert(head);
            head = head->next;
        }
        return false;
    }
};