#include <iostream>
#include <string>
// map和unordermap都是C++字典的头文件，如果需要内部元素自动排序，使用map，
// 不需要排序且为高效的查找问题，使用unordered_map
// #include <map>
#include <unordered_map>
using namespace std;

class Solution {

private:
    unordered_map<char, int> symbolValues = {
        {'I', 1},
        {'V', 5},
        {'X', 10},
        {'L', 50},
        {'C', 100},
        {'D', 500},
        {'M', 1000},
    };
public:
    int romanToInt(string s) {
        int ans = 0;
        int n = s.length();
        for (int i = 0; i < n; i++){
            int value = symbolValues[s[i]];
            if (i < n - 1 && value < symbolValues[s[i + 1]])
                ans -= value;
            else
                ans += value;      
        }
        return ans;
    }
};

int main(int argc, char *argv[])
{
    string roman = "MCMX";
    Solution solution;
    cout << roman << " to int = " << solution.romanToInt(roman) << endl;
    return 0;
}
