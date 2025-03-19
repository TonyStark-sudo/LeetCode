#include <string>
#include <set>
#include <vector>
#include <unordered_set>

using namespace std;
class Solution {
public:
    // 有bug,例如"pwwkew"，应该返回3（wke, kew），下列代码返回的是4（pkew）
    int _lengthOfLongestSubstring(string s) {
        int length = 0;;
        // set<char> set_tep;
        vector<int> set_length;
        for (int i = 0; i < s.size(); i++) {
            for (int j = i + 1; j < s.size(); j++) {
                auto iter_left = &s[i];
                auto iter_right = &s[j];
                set<char> set_tep(iter_left, iter_right);
                set_length.push_back(set_tep.size());
            }
        }

        for (int i = 0; i < set_length.size(); i++) {
            length = std::max(length, set_length[i]);
        }
        return length;
    }

    int lengthOfLongestSubstring(string s) {
        // 定义一个无序集合用来去重
        unordered_set<char> occ;
        set<char> occ;
        int n = s.size();
        // 默认右指针是0，长度为0
        int rk = 0, ans = 0;
        for (int i = 0; i < n; i++) {
            // 当 i 增加时移除 i 前一个元素
            if (i != 0) {
                occ.erase(s[i - 1]);
            }
            // 循环执行的条件是右指针不超过边界，并且集合里面没有右指针对应的元素
            while (rk < n && !occ.count(s[rk])) {
                // 将当前右指针指向的元素丢进集合，右指针开始增加
                occ.insert(s[rk]);
                rk++;
            }

            // 取当前或者（右指针 - 左指针）的最大者为返回值
            ans = max(ans, rk - i);
        }
        return ans;
    }
};