#include <algorithm>
#include <string>
#include <stack>
#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
public:
    vector<char> left{'{', '(', '['};
    vector<char> right{'}', ')',']'};
    bool isValid(string s) {
        stack<char> st;
        for (int i = 0; i < s.length(); i++) {
            if (find(left.begin(), left.end(), s[i]) != left.end()) {
                st.push(s[i]);
            }
            else {
                auto it = find(right.begin(), right.end(), s[i]);
                if (st.top() != left[it - right.begin()]) {
                    return false;
                }
                st.pop();
            }
        }
        return st.empty();
    }
};