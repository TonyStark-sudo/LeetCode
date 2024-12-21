// 定义链表ListNode：
#include <iostream>
#include <vector>

using namespace std;

// 结构体类型
struct ListNode {
    int val; // 数据域
    ListNode *next;  // 指针域，指向下一个链表节点
    // 结构体的构造函数，方便直接对数据域赋值
    ListNode(int x) : val(x), next(NULL) {}
};

int main(int argc, char *argv[])
{
    ListNode* head = new ListNode(2);
    cout << head->val << endl;
    delete head;
    /* code */
    return 0;
}

