#include <vector>

using namespace std;

class Solution {
public:
    int _maxSubArray(vector<int>& nums) {
        int result = nums[0];
        for (int i = 0; i < nums.size(); i++) {
            int sum = 0;
            for (int j = i; j <nums.size(); j++) {
                sum += nums[j];
                if (sum > result) result = sum;
            }
        }
        return result;
    }

    int maxSubArray(vector<int>& nums) {
        int result = nums[0];
        int sum = 0;
        for (int i = 0; i < nums.size(); i++) {
            sum += nums[i];
            result = max(sum, result);
            if (sum < 0) sum = 0;
        }
        return result;
    }
};