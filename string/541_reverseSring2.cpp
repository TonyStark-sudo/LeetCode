#include <string>
#include <algorithm>
using namespace std;

class Solution {
public:
    std::string reverseStr(std::string s, int k) {
        for (int i = 0; i < s.size(); i += (2 * k)) {
            // reverse 函数的第二个参数应该是一个超出要反转范围的迭代器，而不是包含在内的索引
            if (i + k < s.size()) reverse(s.begin() + i, s.begin() + i + k);
            else reverse(s.begin() + i, s.end());
        }

        return s;
    }
};