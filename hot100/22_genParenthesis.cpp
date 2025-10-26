#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <deque>

using namespace std;

class Solution {
public:
    vector<string> genParenthesis(int n) {
        string cur;
        backtracking(n, 0, 0, cur);
        return res;

    }
private:
    vector<string> res;
    void backtracking(int n, int open, int close, string& cur) {
        if (cur.size() == 2 * n) {
            res.push_back(cur);
        }
        if (open < n) {
            cur.push_back('(');
            backtracking(n, open + 1, close, cur);
            cur.pop_back();
        }
        if (close < open) {
            cur.push_back(')');
            backtracking(n, open, close +1, cur);
            cur.pop_back();
        }
    }
};

int main(int argc, char const *argv[])
{
    
    return 0;
}
