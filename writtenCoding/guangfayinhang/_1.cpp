#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>

using namespace std;

class Solution {
public:
    string processQueue(string customerList) {
        queue<int> queueA;
        priority_queue<int> queueB; 

        istringstream iss(customerList);
        int num;
        while (iss >> num) {
            if (num % 2 == 1) {
                queueA.push(num);
            } else {
                queueB.push(num);
            }
        }

        string result;
        int vipCount = 0;
        bool first = true;

        while (!queueB.empty() || !queueA.empty()) {
            while (!queueB.empty() && vipCount < 3) {
                if (!first) {
                    result += " ";
                }
                result += to_string(queueB.top());
                queueB.pop();
                vipCount++;
                first = false;
            }

            if (vipCount == 3 && !queueA.empty()) {
                if (!first) {
                    result += " ";
                }
                result += to_string(queueA.front());
                queueA.pop();
                vipCount = 0;
                first = false;
            }

            if (queueB.empty()) {
                while (!queueA.empty()) {
                    if (!first) {
                        result += " ";
                    }
                    result += to_string(queueA.front());
                    queueA.pop();
                    first = false;
                }
            }
        }

        return result;
    }
};

int main() {
    Solution sol;
    string input;
    getline(cin, input);
    string output = sol.processQueue(input);
    cout << "\"" << output << "\"" << endl;
    return 0;
}