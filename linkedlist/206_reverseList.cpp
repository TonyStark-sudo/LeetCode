#include <iostream>

using namespace std;

struct NodeList
{
    /* data */
    int val;
    NodeList* next;
    NodeList(int x) : val(x), next(NULL) {};
};

class Solution {
    public:
    // 可以考虑翻转指针指向，而不是交换数值
    NodeList* reverseList(NodeList* head) {
        /*双指针法
        定义双指针pre、cur
        */
       NodeList* pre = NULL;
       while (head != NULL) {
        NodeList* temp = head->next;
        head->next = pre;
        pre = head;
        head = temp;
       }

       return pre;
    }

        /*递归方法
        首先定义reverse函数
        */
    NodeList* reverse(NodeList* &cur, NodeList* &pre) {
        if (cur == NULL) return pre;
        NodeList* temp = cur->next;
        cur->next = pre;
        return reverse(temp, cur);
    }

    NodeList* _reverseList(NodeList* head) {
        NodeList* pre = NULL;
        return reverse(head, pre);
    }
};