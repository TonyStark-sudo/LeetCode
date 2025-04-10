#include <vector>

class Solution {
public:
    int climbStairs(int n) {
        std::vector<int> res(n + 1);
        if (n <= 2) return n;
        res[1] = 1;
        res[2] = 2;
        for (int i = 3; i <= n; i++) {
            res[i] = res[i - 1] + res[i - 2];
        }
        return res[n];
    }

    int spaceOptimizedClimb(int n) {
        if (n <= 2) return n;
        int tmp1 = 1;
        int tmp2 = 2;
        int sum;
        for (int i = 3; i <= n; i++) {
            sum = tmp1 + tmp2;
            tmp1 = tmp2;
            tmp2 = sum;
        }
        return sum; 
    }
};