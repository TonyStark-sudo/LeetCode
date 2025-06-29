#include <vector>
#include <iostream>

using namespace std;
class Solution {
public:
    bool canJump(vector<int>& nums) {
        int max_index_canJump = 0;
        for (int i = 0; i <= nums.size(); i++) {
            int index_canJump = nums[i] + i;
            max_index_canJump = max(index_canJump, max_index_canJump);
            std::cout << "max_index_canJump: " << max_index_canJump << std::endl;
            if (max_index_canJump >= nums.size() - 1) return true;
        }
        return false;
    }
};

int main(int argc, char const *argv[])
{
    vector<int> nums{3, 2, 1, 0, 4};
    Solution solution;
    bool canJump = solution.canJump(nums);
    return 0;
}
