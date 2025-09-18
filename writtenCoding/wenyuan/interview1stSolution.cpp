#include <iostream>
#include <vector>

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
};

class Solution {
public:
    std::pair<ListNode*, ListNode*> splitList(ListNode* head) {
        if (!head || !head->next) {
            return {head, nullptr};
        }
        
        ListNode* slow = head;
        ListNode* fast = head->next; // 从第二个节点开始，确保奇数时前半部分多一个
        
        // 使用快慢指针找到分割点
        // 当链表长度为奇数时，slow 会停在中间节点（前半部分的最后一个）
        // 当链表长度为偶数时，slow 会停在前半部分的最后一个节点
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        
        // 记录第二部分的头节点
        ListNode* secondHead = slow->next;
        
        // 断开连接
        slow->next = nullptr;
        
        return {head, secondHead};
    } 
};

// 工具函数
class ListUtils {
public:
    static ListNode* createList(const std::vector<int>& values) {
        if (values.empty()) return nullptr;
        
        ListNode* head = new ListNode(values[0]);
        ListNode* current = head;
        
        for (int i = 1; i < values.size(); i++) {
            current->next = new ListNode(values[i]);
            current = current->next;
        }
        
        return head;
    }
    
    static void printList(ListNode* head, const std::string& label = "") {
        if (!label.empty()) {
            std::cout << label << ": ";
        }
        
        std::cout << "[";
        while (head) {
            std::cout << head->val;
            if (head->next) std::cout << ",";
            head = head->next;
        }
        std::cout << "]";
    }
    
    static void deleteList(ListNode* head) {
        while (head) {
            ListNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    static int getLength(ListNode* head) {
        int length = 0;
        while (head) {
            length++;
            head = head->next;
        }
        return length;
    }
};

int main(int argc, char const *argv[])
{
    Solution solution;
    
    std::cout << "=== 链表等分测试用例 ===" << std::endl << std::endl;
    
    // 测试用例1: 偶数长度链表 [1,2,3,4,5,6]
    std::cout << "测试用例1 - 偶数长度链表:" << std::endl;
    std::vector<int> values1 = {1, 2, 3, 4, 5, 6};
    ListNode* head1 = ListUtils::createList(values1);
    ListUtils::printList(head1, "原链表");
    std::cout << " (长度: " << ListUtils::getLength(head1) << ")" << std::endl;
    
    auto result1 = solution.splitList(head1);
    ListUtils::printList(result1.first, "第一部分");
    std::cout << " (长度: " << ListUtils::getLength(result1.first) << ")" << std::endl;
    ListUtils::printList(result1.second, "第二部分");
    std::cout << " (长度: " << ListUtils::getLength(result1.second) << ")" << std::endl;
    
    // 清理内存
    ListUtils::deleteList(result1.first);
    ListUtils::deleteList(result1.second);
    
    std::cout << std::endl;
    
    // 测试用例2: 奇数长度链表 [1,2,3,4,5]
    std::cout << "测试用例2 - 奇数长度链表:" << std::endl;
    std::vector<int> values2 = {1, 2, 3, 4, 5};
    ListNode* head2 = ListUtils::createList(values2);
    ListUtils::printList(head2, "原链表");
    std::cout << " (长度: " << ListUtils::getLength(head2) << ")" << std::endl;
    
    auto result2 = solution.splitList(head2);
    ListUtils::printList(result2.first, "第一部分");
    std::cout << " (长度: " << ListUtils::getLength(result2.first) << ")" << std::endl;
    ListUtils::printList(result2.second, "第二部分");
    std::cout << " (长度: " << ListUtils::getLength(result2.second) << ")" << std::endl;
    
    // 清理内存
    ListUtils::deleteList(result2.first);
    ListUtils::deleteList(result2.second);
    
    std::cout << std::endl;
    
    // 测试用例3: 单个节点 [1]
    std::cout << "测试用例3 - 单个节点:" << std::endl;
    std::vector<int> values3 = {1};
    ListNode* head3 = ListUtils::createList(values3);
    ListUtils::printList(head3, "原链表");
    std::cout << " (长度: " << ListUtils::getLength(head3) << ")" << std::endl;
    
    auto result3 = solution.splitList(head3);
    ListUtils::printList(result3.first, "第一部分");
    std::cout << " (长度: " << ListUtils::getLength(result3.first) << ")" << std::endl;
    ListUtils::printList(result3.second, "第二部分");
    if (result3.second) {
        std::cout << " (长度: " << ListUtils::getLength(result3.second) << ")" << std::endl;
    } else {
        std::cout << " (空链表)" << std::endl;
    }
    
    // 清理内存
    ListUtils::deleteList(result3.first);
    ListUtils::deleteList(result3.second);
    
    std::cout << std::endl;
    
    // 测试用例4: 两个节点 [1,2]
    std::cout << "测试用例4 - 两个节点:" << std::endl;
    std::vector<int> values4 = {1, 2};
    ListNode* head4 = ListUtils::createList(values4);
    ListUtils::printList(head4, "原链表");
    std::cout << " (长度: " << ListUtils::getLength(head4) << ")" << std::endl;
    
    auto result4 = solution.splitList(head4);
    ListUtils::printList(result4.first, "第一部分");
    std::cout << " (长度: " << ListUtils::getLength(result4.first) << ")" << std::endl;
    ListUtils::printList(result4.second, "第二部分");
    std::cout << " (长度: " << ListUtils::getLength(result4.second) << ")" << std::endl;
    
    // 清理内存
    ListUtils::deleteList(result4.first);
    ListUtils::deleteList(result4.second);
    
    std::cout << std::endl;
    
    // 测试用例5: 空链表
    std::cout << "测试用例5 - 空链表:" << std::endl;
    ListNode* head5 = nullptr;
    std::cout << "原链表: [] (长度: 0)" << std::endl;
    
    auto result5 = solution.splitList(head5);
    if (result5.first) {
        ListUtils::printList(result5.first, "第一部分");
        std::cout << " (长度: " << ListUtils::getLength(result5.first) << ")" << std::endl;
    } else {
        std::cout << "第一部分: [] (空链表)" << std::endl;
    }
    
    if (result5.second) {
        ListUtils::printList(result5.second, "第二部分");
        std::cout << " (长度: " << ListUtils::getLength(result5.second) << ")" << std::endl;
    } else {
        std::cout << "第二部分: [] (空链表)" << std::endl;
    }
    
    std::cout << std::endl << "=== 所有测试用例完成 ===" << std::endl;
    
    return 0;
}
