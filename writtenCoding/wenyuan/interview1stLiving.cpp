#include <iostream>

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

class Solution {
public:
    std::pair<ListNode*, ListNode*> splitList(ListNode* head) {
        if (!head || !head->next) {
            return {head, nullptr};
        }
        ListNode* fast = head;
        ListNode* slow = head;
        ListNode* prev = nullptr;
        while (fast && fast->next) {
            prev = slow;
            slow = slow->next;
            fast = fast->next->next;
        }
        if (prev) {
            prev->next = nullptr;
        }
        // ListNode* tmp = head;
        // int listSize = 0;
        // while (!tmp) {
        //     listSize++;
        //     tmp = tmp->next;
        // }
        // if (listSize % 2) {
        //     return {head, slow};
        // }
        return {head, slow};
    }

    
    std::pair<ListNode*, ListNode*> splitListNew(ListNode* head) {
    if (!head || !head->next) {
        return {head, nullptr};
    }
    
    // 使用快慢指针找到中点
    // 为了确保奇数时左边多一个，fast从head->next开始
    ListNode* fast = head->next;
    ListNode* slow = head;
    
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    // slow现在指向左半部分的最后一个节点
    ListNode* rightHalf = slow->next;
    slow->next = nullptr;  // 断开连接
    
    return {head, rightHalf};
}
};

void printList(ListNode* head) {
    while (head) {
        std::cout << head->val << " ";
        head = head->next;
    }
    std::cout << std::endl;
}

// 释放链表内存
void deleteList(ListNode* head) {
    while (head) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

int main(int argc, char const *argv[])
{
    ListNode* head = new ListNode(1);
    ListNode* cur = head;
    for (int i = 2; i <= 8; i++) {
        cur->next = new ListNode(i);
        cur = cur->next;
    }
    // ListNode* cur = head;
    printList(head);
    Solution solution;
    // std::pair<ListNode*, ListNode*> splitedLists = solution.splitList(head);
    std::pair<ListNode*, ListNode*> splitedLists = solution.splitListNew(head);
    printList(splitedLists.first);
    printList(splitedLists.second);

    deleteList(splitedLists.first);
    deleteList(splitedLists.second);
    return 0;
}


