#include <string>
#include <vector>
#include <queue>
#include <unordered_set>


using namespace std;

class Solution {
public:
    vector<string> removeInvalidParentheses(string s) {
        vector<string> res;
        if (s.empty()) return {};
        queue<string> q;
        unordered_set<string> vis;
        q.push(s);
        vis.insert(s);

        bool found = false;

        while (!q.empty()) {
            int sz = q.size();
            for (int _ = 0; _ < sz; ++_) {
                string cur = q.front();
                q.pop();

                if (isValid(cur)) {
                    res.push_back(cur);
                    found = true;
                }
                if (found) continue;

                for (int i = 0; i < cur.size(); i++) {
                    char c = cur[i];
                    if (c != '(' && c != ')') continue;
                    if (i > 0 && cur[i] == cur[i - 1]) continue;
                    string nxt = cur.substr(0, i) + cur.substr(i + 1);
                    if (!vis.count(nxt)) {
                        vis.insert(nxt);
                        q.push(nxt);
                    }
                }
            }
        }
        if (res.empty()) res.push_back("");
        return res;
    }
private:
    bool isValid(const string& s) {
        int bal = 0;
        for (char c : s) {
            if (c == '(') ++bal;
            else if (c == ')') {
                if (bal == 0) return false;
                --bal;
            }
        }
        return bal == 0;
    }
};

// ...existing code...
class SolutionDFS {
public:
    vector<string> removeInvalidParentheses(string s) {
        // 预处理：需要删除的左右括号数量
        int leftRem = 0, rightRem = 0;
        for (char c : s) {
            if (c == '(') {
                ++leftRem;
            } else if (c == ')') {
                if (leftRem > 0) --leftRem;
                else ++rightRem;
            }
        }
        string path;
        unordered_set<string> resSet;  // 去重
        dfs(s, 0, leftRem, rightRem, 0, path, resSet);
        return vector<string>(resSet.begin(), resSet.end());
    }

private:
    // i: 当前下标；leftRem/rightRem：还需删除多少左/右括号；open：当前未闭合的左括号数
    void dfs(const string& s, int i, int leftRem, int rightRem, int open,
             string& path, unordered_set<string>& out) {
        if (leftRem < 0 || rightRem < 0 || open < 0) return;

        if (i == (int)s.size()) {
            if (leftRem == 0 && rightRem == 0 && open == 0) {
                out.insert(path);
            }
            return;
        }

        char c = s[i];
        if (c != '(' && c != ')') {
            // 非括号字符必须保留
            path.push_back(c);
            dfs(s, i + 1, leftRem, rightRem, open, path, out);
            path.pop_back();
            return;
        }

        // 选择删除当前括号
        if (c == '(' && leftRem > 0) {
            dfs(s, i + 1, leftRem - 1, rightRem, open, path, out);
        }
        if (c == ')' && rightRem > 0) {
            dfs(s, i + 1, leftRem, rightRem - 1, open, path, out);
        }

        // 选择保留当前括号
        if (c == '(') {
            path.push_back('(');
            dfs(s, i + 1, leftRem, rightRem, open + 1, path, out);
            path.pop_back();
        } else { // c == ')'
            if (open > 0) {
                path.push_back(')');
                dfs(s, i + 1, leftRem, rightRem, open - 1, path, out);
                path.pop_back();
            }
        }
    }
};

int main(int argc, char const *argv[])
{
    
    return 0;
}
