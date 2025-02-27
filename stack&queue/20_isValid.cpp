#include <string>
#include <vector>
#include <algorithm>
#include <stack>

class Solution {
public:

    int search(std::string s, char target) {
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] == target) return true;
        }
        return false;
    }

    bool isValid(std::string s) {
        int left_count = 0;
        int right_count = 0;
        std::stack<char> stack_tmp;        
        std::vector<char> left, right;
        left.push_back('(');
        left.push_back('{');
        left.push_back('[');
        right.push_back(')');
        right.push_back('}');
        right.push_back(']');

        // int j = 1;
        // for (size_t i = 0; i < s.size(); i++) {
        //     for (size_t j = i + 1; j < s.size(); j++) {
        //         if (search(left, j) && search(right, i))
        //             return false;
        //     }
        // }
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] == '(') stack_tmp.push(')');
            else if (s[i] == '[') stack_tmp.push(']');
            else if (s[i] == '{') stack_tmp.push('}');

            else if (stack_tmp.empty() || s[i] != stack_tmp.top()) return false;
            else stack_tmp.pop();
        }

        return stack_tmp.empty();
    }
};

