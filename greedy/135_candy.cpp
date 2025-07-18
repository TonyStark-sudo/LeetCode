#include <vector>

using namespace std;
class Solution {
public:
    int _candy(vector<int>& ratings) {
        vector<int> candyVec(ratings.size(), 1);
        // 从前向后
        for (int i = 1; i < ratings.size(); i++) {
            if (ratings[i] > ratings[i - 1]) candyVec[i] = candyVec[i - 1] + 1;
        }
        // 从后向前
        for (int i = ratings.size() - 2; i >= 0; i--) {
            if (ratings[i] > ratings[i + 1] ) {
                candyVec[i] = max(candyVec[i], candyVec[i + 1] + 1);
            }
        }
        // 统计结果
        int result = 0;
        for (int i = 0; i < candyVec.size(); i++) result += candyVec[i];
        return result;
    }

    int candy(std::vector<int>& ratings) {
        int sum = 0;
        std::vector<int> candy_per_child(ratings.size(), 1);
        for (int i = 1; i < ratings.size(); i++) {
            if (ratings[i] > ratings[i - 1]) {
                candy_per_child[i] = candy_per_child[i - 1] + 1;
            }
        }
        for (int i = ratings.size() - 2; i >= 0; i--) {
            if (ratings[i] > ratings[i + 1]) {
                candy_per_child[i] = max(candy_per_child[i], candy_per_child[i + 1] + 1);
            }
        }
        for (int i = 0; i < candy_per_child.size(); i++) sum += candy_per_child[i];
        return sum;
    }
};