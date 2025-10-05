#include <vector>
#include "ListNode/listnode.h"

class MySolution {
private:
    bool isPd(const std::vector<int>& nums) {
        for (int i = 0; i < nums.size() / 2; i++) {
            if (nums[i] != nums[nums.size() - 1 - i]) {
                return false;
            }
        }
        return true;
    }
public:
    bool isPalindrome(ListNode* head) {
        ListNode* tmp = head;
        std::vector<int> values;
        while (tmp) { 
            values.push_back(tmp->val);
            tmp = tmp->next;
        }
        return isPd(values);
    }
};

class Solution {
    ListNode* frontPointer;
public:
    bool recursivelyCheck(ListNode* currentNode) {
        if (currentNode != nullptr) {
            if (!recursivelyCheck(currentNode->next)) {
                return false;
            }
            if (currentNode->val != frontPointer->val) {
                return false;
            }
            frontPointer = frontPointer->next;
        }
        return true;
    }

    bool isPalindrome(ListNode* head) {
        frontPointer = head;
        return recursivelyCheck(head);
    }
};