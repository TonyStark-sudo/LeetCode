// 一句话总结思路：使用startIndex判断分割字符串的起始位置，进行回溯分割子串并判断是否为回文串
// 获取子串使用string.substr(pos, len)方法
#include <string>
#include <vector>
using namespace std;
class Solution {
private:
    vector<vector<string>> results;
    vector<string> path;
    bool isValid(const string& sub_str) {
        for (int i = 0; i < sub_str.length() / 2; i++) {
            if (sub_str[i] != sub_str[sub_str.size() - 1 - i]) {
                return false;
            }
        }
        return true;
    }
    void backtracking(const string& s, int startIndex) {
        if (startIndex >= s.size()) {
            results.push_back(path);
            return ;
        }
        for (int i = startIndex; i < s.size(); i++) {
            // string.substr(pos, len) pos:子字符串的起始下标 len:要截取的长度
            string sub_str = s.substr(startIndex, i - startIndex + 1);
            if (isValid(sub_str)) {
                path.push_back(sub_str);
                backtracking(s, i + 1);
                path.pop_back();
            }
        }
    }
public:
    vector<vector<string>> partition(string s) {
        backtracking(s, 0);
        return results;
    }
};