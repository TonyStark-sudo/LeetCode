#include <vector>
#include <iostream>

int getMax(std::vector<int>& nums, int left, int right) {
    int maxValue = nums[left];
    for (int i = left + 1; i <= right; i++) {
        maxValue = std::max(maxValue, nums[i]);
    }
    return maxValue;
}

class Solution {
public:
    int jump(std::vector<int>& nums) {
        // 如果nums的 size 是1那么不需要进行跳跃
        if (nums.size() == 1) return 0;
        // 记录当前位置能跳的最远的下标
        int curDistance = 0;
        // 最小步数
        int ans = 0;
        // 如果curDistance < nums.size() - 1 那么需要计算下一步最远的覆盖距离
        int nextDistance = 0;
        for (int i = 0; i < nums.size(); i++) {
            // 每到一个元素算出从当前元素能跳的最远的下标, 记为nextDistance
            nextDistance = std::max(nums[i] + i, nextDistance);
            // 如果 i 遍历到了当前的最远距离，如果  < nums.size() - 1 那么 ans 必须 + 1
            if (i == curDistance) {
                // if (curDistance != nums.size() - 1) {
                // 条件这样写感觉更好理解
                if (curDistance <= nums.size() - 1) {
                    ans++;
                    curDistance = nextDistance;
                    // 更新完curDistance之后，还要判断nextDistance的距离
                    if (nextDistance >= nums.size() - 1) break;

                }
                else break;
            }
        }

        return ans;
    }
};

int main(int argc, char *argv[])
{
    std::vector<int> nums{1, 3, 4, 5, 6, 3, 23, 6};
    std::cout << "index0-index6 maxValue: " << getMax(nums, 0, 6) << "\n";
    /* code */
    return 0;
}
