#include <vector>
#include <algorithm>

class Solution {
public:
    int _wiggleMaxLength(std::vector<int>& nums) {
        if (nums.size() < 2) {
            return nums.size();
        }
        int result = nums.size();
        for (int i = 0; i < nums.size(); i++) {
            if (i > 0 && i + 1 < nums.size()) {
                if (nums[i] >= nums[i - 1] && nums[i] <= nums[i + 1]) {
                    result--;
                }
                else if (nums[i] <= nums[i - 1] && nums[i] >= nums[i + 1]) {
                    result--;
                }
            }
        }
        return result;
    }
    int __wiggleMaxLength(std::vector<int>& nums) {
        if (nums.size() < 2) return nums.size();
        int up =1, down = 1;
        for (int i = 1; i< nums.size(); i++) {
            if (nums[i] > nums[i - 1]) up = down + 1;
            else if (nums[i] < nums[i - 1]) down = up + 1;
        }
        return std::max(up, down);
    }
    
    int wiggleMaxLength(std::vector<int>& nums) {
        if (nums.size() < 2) return nums.size();
        int prediff = 0, curdiff = 0;
        int result = 1;
        for (int i = 0; i < nums.size() - 1; i++) {
            curdiff = nums[i + 1] - nums[i];
            if ((curdiff > 0 && prediff <= 0) || (curdiff <0 && prediff >= 0)) {
                result++;
                prediff = curdiff;
            }
        }
        return result;
    }
};