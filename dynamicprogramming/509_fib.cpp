#include <vector>

class Solution {
public:
    int fib(int n) {
        int f1 = 0, f2 = 1;
        std::vector<int> tmp_vec;
        tmp_vec.push_back(f1);
        tmp_vec.push_back(f2);

        if (n > 1) {
            while(tmp_vec.size() <= n) {
                // 不能使用n，若n很大，并无tmp_vec[n - 1]，tmp_vec[n - 2]导致数组越界
                // tmp_vec.push_back(tmp_vec[n - 1] + tmp_vec[n - 2]);
                tmp_vec.push_back(tmp_vec[tmp_vec.size() - 1] + tmp_vec[tmp_vec.size() - 2]);
            }
        }

        return tmp_vec[n];
    }
};