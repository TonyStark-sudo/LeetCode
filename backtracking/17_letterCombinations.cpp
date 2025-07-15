#include <string>
#include <vector>
#include <map>

using namespace std;
class Solution {
private:
    map<char, string> digit2letters = {
        {'2', "abc"},
        {'3', "def"},
        {'4', "ghi"},
        {'5', "jkl"},
        {'6', "mno"},
        {'7', "pqrs"},
        {'8', "tuv"},
        {'9', "wxyz"}
    };
    vector<string> result;
    string path;
    void backtracking(const string& digits, int startIndex) {
        if (path.size() == digits.size()) {
            result.push_back(path);
            return ;
        }
        string letters = digit2letters[digits[startIndex]];
        for (int i = 0; i < letters.size(); i++) {
            path.push_back(letters[i]);
            backtracking(digits, startIndex + 1);
            path.pop_back();
        }

    }
public:
    vector<string> letterCombinations(string digits) {
        if (digits.empty()) return {};
        backtracking(digits, 0);
        return result;
    }
};