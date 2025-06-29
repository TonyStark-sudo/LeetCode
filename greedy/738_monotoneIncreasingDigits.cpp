#include <string>

class Solution {
private:
    // 判断一个数字的各位上是否是递增
    bool checkNum(int num) {
        int max = 10;
        while (num) {
            int t = num % 10;
            if (max >= t) max = t;
            else return false;
            num = num / 10;
        }
        return true;
    }
public:
    int _monotoneIncreasingDigits(int N) {
        for (int i = N; i > 0; i--) { // 从大到小遍历
            if (checkNum(i)) return i;
        }
        return 0;
    }

    int monotoneIncreasingDigits(int N) {
        std::string num = std::to_string(N);
        int flag = num.size();
        for (size_t i{num.length() - 1}; i > 0; i--) {
            if (num[i] < num[i - 1]) {
                // num[i] = '9';
                flag = i;
                num[i - 1]--;
            }
        }
        for (int i = flag; i < num.size(); i++) {
            num[i] = '9';
        }
        return std::stoi(num);
    }
};