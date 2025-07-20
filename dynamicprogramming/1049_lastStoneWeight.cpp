#include <vector>
using namespace std;

class Solution {
public:
    int lastStoneWeightII(vector<int>& stones) {
        // sort(stones.begin(), stones.end());
        // for (int i = 1; i < stones.size(); i++) {
        //     stones[i] = stones[i] - stones[i - 1];
        // }
        // return stones[stones.size() - 1];
        int sum = 0; 
        for (int i = 0; i < stones.size(); i++) {
            sum += stones[i];
        }

        int dp[1501] = {0};
        for (int i = 0; i < stones.size(); i++) {
            for (int j = sum / 2; j >= stones[i]; j--) {
                dp[j] = max(dp[j], dp[j - stones[i]] + stones[i]);
            }
        }
        // return sum - sum / 2 - dp[sum / 2];
        return sum - 2 * dp[sum / 2];
    }
};