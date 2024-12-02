#include <iostream>
#include <vector>

using namespace std;
struct ListNode
{
    int value;
    ListNode *next;
    ListNode(int x) : value(x), next(NULL) {} // 节点的构造函数

    /* data */
};

class Solution {
public:
// 第一版
    ListNode* removeElement(ListNode* head, int val) {
        ListNode* temp = head;
        while (temp->next != NULL) {
            if (head->value == val) {
                head = head->next;
                temp = head;
            }
                
            if (temp->value == val) {
                temp->next = temp->next->next;
                temp = temp->next;
            }

            else temp = temp->next;
            if (temp->next == NULL && temp->value == val)
                temp =NULL;

        }
        return head;
    }
// 第二版
    ListNode* _removeElement(ListNode* head, int val) {
        
        // 若头结点为空，操作空指针编译会报错
        while (head != NULL && head->value == val) {
            ListNode* temp = head;
            head = head->next;
            // C++需要手动内存释放
            delete(temp);
        }

        ListNode* cur = head;
        // 注意不要操作空指针，所以要判断 temp != NULL && temp->next != NULL
        while (cur != NULL && cur->next != NULL) {
            if (cur->next->value == val) {
                ListNode* temp = cur->next;
                cur->next = cur->next->next;
                // cur = cur->next;
                delete(temp);
            }

            else cur = cur->next;
        }

        return head;
    }
// 第三版
    ListNode* __removeElement(ListNode* head, int target) {
        ListNode* dummyHead = new ListNode(0);
        dummyHead->next = head;
        ListNode* cur = dummyHead;
        while (cur->next != NULL) {
            if (cur->next->value == target) {
                ListNode* temp = cur->next;
                cur->next = cur->next->next;
                delete(temp);
            }
            else cur = cur->next;
        }
        head = dummyHead->next;
        delete(dummyHead);
        return head;
    }
};


int main(int argc, char *argv[])
{
    ListNode* head = new ListNode(19);
    cout << "data is: " << head->value << endl;
    /* code */
    return 0;
}