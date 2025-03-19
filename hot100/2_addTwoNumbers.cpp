#include "listnode.h"

class Solution {
public:
    // 只适用于两个数位数相等
    ListNode* _addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* res = new ListNode(0);
        ListNode* tem = res;
        bool flag = false;
        while (l1 != NULL || l2 != NULL || flag) {
            int sum = l1->val + l2->val;
            if (flag) {
                if (sum + 1 < 10) {
                    tem->next = new ListNode(sum + 1);
                    flag = false;
                }
                
                else {
                    tem->next = new ListNode((sum + 1) % 10);
                    flag = true;
                }
            }
            else {
                if (sum < 10) {
                    tem->next = new ListNode(sum);
                    flag = false;
                }
                else {
                    tem->next = new ListNode(sum % 10);
                    flag = true;
                }
            }
            l1 = l1->next;
            l2 = l2->next;
            tem = tem->next;
        }
        return res->next;
    }

    // 答案
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* dummyHead = new ListNode(0);
        ListNode* current = dummyHead;
        int carry = 0;
        
        while (l1 != NULL || l2 != NULL || carry != 0) {
            int sum = carry;
            if (l1 != NULL) {
                sum += l1->val;
                l1 = l1->next;
            }
            if (l2 != NULL) {
                sum += l2->val;
                l2 = l2->next;
            }
            carry = sum / 10;
            current->next = new ListNode(sum % 10);
            current = current->next;
        }
        
        return dummyHead->next;
    }
};