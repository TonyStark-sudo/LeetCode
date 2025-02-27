#include <stack>
#include <vector>
#include <string>

class Solution {
public:
    int evalRPN(std::vector<std::string>& tokens) {
        std::stack<int> st;
        for (size_t i = 0; i < tokens.size(); i++) {
            if (tokens[i] == "+" || tokens[i] == "-" || tokens[i] == "*" || tokens[i] == "/") {
                int num1 = st.top();
                st.pop();
                int num2 = st.top();
                st.pop();

                if (tokens[i] == "+") st.push(num2 + num1);
                else if (tokens[i] == "-") st.push(num2 - num1);
                else if (tokens[i] == "*") st.push(num2 * num1);
                else if (tokens[i] == "/") st.push(num2 / num1);
            }

            else {
                // st.push(int(tokens[i][0]));
                st.push(stoi(tokens[i]));
            }
        }

        return st.top();
    }
};