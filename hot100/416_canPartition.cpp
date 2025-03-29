#include <vector>
#include <algorithm>

class Solution {
public:
    bool canPartition(std::vector<int>& nums) {
        int sum = 0;
        for (int i = 0; i < nums.size(); i++) {
            sum += nums[i];
        }
        if (sum % 2) return false;
        std::sort(nums.begin(), nums.end());
        int tmp = 0;
        for (int i = 0; i < nums.size(); i++) {
            tmp += nums[i];
            if (tmp == sum / 2) return true; 
        }
        return false;
    }
    bool _canPatition(std::vector<int>& nums) {
        std::vector<int> dp(20001, 0);
        int sum = 0;
        for (int i = 0; i < nums.size(); i++) {
            sum += nums[i];
        }
        if (sum % 2) return false;
        int tmp = sum / 2;

        for (int i = 0; i < nums.size(); i++) {
            for (int j = tmp; j >= nums[i]; j--) {
                dp[j] = std::max(dp[j], dp[j - nums[i]] + nums[i]);
            }
        }
        if (dp[tmp] == tmp) return true;
        return false;
    }
};