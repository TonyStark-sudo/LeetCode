#include <stack>
#include <string>
#include <unordered_map>

using namespace std;

class Solution {
public:
    bool isValid(string s) {
        stack<char> st;
        unordered_map<char, char> m = {{')', '('}, {']', '['}, {'}', '{'}};
        for (char c : s) {
            if (m.count(c)) {
                if (st.empty() || st.top() != m[c]) return false;
                st.pop();
            }
            else st.push(c);
        }
        return st.empty();
    }
};
int main(int argc, char const *argv[])
{

    return 0;
}
