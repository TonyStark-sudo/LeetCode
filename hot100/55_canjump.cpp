#include <vector>

class Solution {
public:
    bool canjump(std::vector<int>& nums) {
        // cover是指覆盖最大范围的索引
        int cover = 0;
        if (nums.size() == 1) return true;
        // 为什么循环终止的条件是 i <= cover ? 
        // 因为大于cover的索引无论怎样也是跳不过去的，所以 i 自增到大于cover是没有意义的
        for (int i = 0; i <= cover; i++) {
            cover = std::max(i + nums[i], cover);
            if (cover >= nums.size() - 1) return true;
        }
        return false;        
    }
};