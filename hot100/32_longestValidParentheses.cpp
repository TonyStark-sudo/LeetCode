#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    int longestValidParentheses(string s) {
        int ans = 0;
        vector<int> st;
        st.push_back(-1);
        for (int i = 0; i < s.size(); i++) {
            if (s[i] == '(') {
                st.push_back(i);
            }
            else {
                st.pop_back();
                if (st.empty()) {
                    st.push_back(i);
                } else {
                    ans = max(ans, i - st.back());
                }
            }
        }
        return ans;
    }
};

int main(int argc, char const *argv[])
{
    
    return 0;
}
