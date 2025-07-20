#include <vector>
using namespace std;

class Solution_use_dfs {
private:
    int cur_sum = 0;
    int half_sum_;
    void backtracking(const vector<int>& nums, int startIndex, bool& found) {
        if (startIndex >= nums.size() || cur_sum >= half_sum_) {
            if (cur_sum == half_sum_) found = true;
            return ;
        }
        for (int i = startIndex; i < nums.size(); i++) {
            cur_sum += nums[i];
            backtracking(nums, i + 1, found);
            cur_sum -= nums[i];
        }
    }
public:
    bool canPartition(vector<int>& nums) {
        int sum = 0;
        for (int i= 0; i < nums.size(); ++i) {
            sum += nums[i];
        }
        if (sum % 2) return false;
        half_sum_ = sum / 2;
        bool found = false;
        backtracking(nums, 0, found);
        return found;
    }
};

class Solution_use_dp {
public:
    bool canPartition(vector<int>& nums) {
        int sum = 0; 
        for (int i = 0; i < nums.size(); ++i) {
            sum += nums[i];
        }
        if (sum % 2) return false;
        // int dp[10000] = {0};
        int dp[10001] = {0};
        for (int i = 0; i < nums.size(); ++i) {
            for (int j = sum / 2; j >= nums[i]; j--) {
                dp[j] = max(dp[j], dp[j - nums[i]] + nums[i]);
            }
        }

        return dp[sum / 2] == sum / 2;
    }
};