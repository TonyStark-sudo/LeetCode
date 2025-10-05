#include <vector>
#include <stack>

using namespace std;

class MySolution {
public:
    vector<int> dailyTemperatures(vector<int>& temperatures) {
        vector<int> ans(temperatures.size(), 0);
        for (int i = 0; i < temperatures.size(); i++) {
            for (int j = i + 1; j < temperatures.size(); j++) {
                if (temperatures[j] <= temperatures[i]) continue;
                if (temperatures[j] > temperatures[i]) {
                    ans[i] = j - i;
                    break;
                }
            }
        }
        return ans;
    }
};

// temperatures = [73,74,75,71,69,72,76,73]
class Solution {
public:
    vector<int> dailyTemperatures(vector<int>& temperatures) {
        vector<int> ans(temperatures.size(), 0);
        stack<int> s;
        for (int i = 0; i < temperatures.size(); i++) {
            printf("Day %d, Temp %d\n", i, temperatures[i]);
            while (!s.empty() && temperatures[i] > temperatures[s.top()]) {
                int previousIndex = s.top();
                s.pop();
                ans[previousIndex] = i - previousIndex;
                printf("  Pop index %d (Temp %d), set ans[%d]=%d\n", previousIndex, temperatures[previousIndex], previousIndex, i - previousIndex);
            }
            s.push(i);
            printf("  Push index %d\n", i);
        }
        return ans;
    }
};
// 测试用例和主函数
#include <iostream>
int main() {
    vector<int> temperatures = {73, 74, 75, 71, 69, 72, 76, 73};
    Solution sol;
    vector<int> result = sol.dailyTemperatures(temperatures);
    std::cout << "Result: ";
    for (int n : result) std::cout << n << " ";
    std::cout << std::endl;
    return 0;
}