// 有效的括号
// 给定一个只包括 '('，')'，'{'，'}'，'['，']' 的字符串s，判断字符串是否有效。
// 注：字符串需要有序且成对存在，例如'([])'是有效字符串，'[{]'和'[{]}'为无效字符串。
// 1 <= s.length <= 104
// s 仅由括号 '()[]{}' 组成

#include <iostream>
#include <string>
#include <stack>
#include <unordered_map>
#include <vector>
using namespace std;

class Solution {
public:
    bool isValid(string s) {
        // 栈用于存储左括号
        stack<char> st;
        
        // 映射右括号到对应的左括号
        unordered_map<char, char> mapping = {
            {')', '('},
            {'}', '{'},
            {']', '['}
        };
        
        for (char c : s) {
            // 如果是右括号
            if (mapping.find(c) != mapping.end()) {
                // 栈为空或栈顶元素不匹配
                if (st.empty() || st.top() != mapping[c]) {
                    return false;
                }
                st.pop(); // 匹配成功，弹出栈顶元素
            } else {
                // 左括号，直接入栈
                st.push(c);
            }
        }
        
        // 栈为空说明所有括号都匹配
        return st.empty();
    }
};

// 简化版本 - 不使用哈希表
class Solution2 {
public:
    bool isValid(string s) {
        stack<char> st;
        for (char c : s) {
            if (c == '(' || c == '{' || c == '[') {
                // 左括号入栈
                st.push(c);
            } else {
                // 右括号
                if (st.empty()) return false;
                
                char top = st.top();
                st.pop();
                
                // 检查是否匹配
                if ((c == ')' && top != '(') ||
                    (c == '}' && top != '{') ||
                    (c == ']' && top != '[')) {
                    return false;
                }
            }
        }
        
        return st.empty();
    }
};

int main() {
    Solution solution;
    
    // 测试用例
    vector<string> testCases = {
        "()",           // true
        "()[]{}",       // true
        "(]",           // false
        "([)]",         // false
        "{[]}",         // true
        "(((",          // false
        ")))",          // false
        "",             // true (空字符串视为有效)
        "([])",         // true
        "[{]}"          // false
    };
    
    cout << "有效括号测试结果：" << endl;
    for (int i = 0; i < testCases.size(); i++) {
        string s = testCases[i];
        bool result = solution.isValid(s);
        cout << "\"" << s << "\" -> " << (result ? "true" : "false") << endl;
    }
    
    return 0;
}