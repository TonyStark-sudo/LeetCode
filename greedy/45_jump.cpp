#include <vector>
#include <iostream>
#include <memory>

using namespace std;

class Solution {
public:
    int _jump(vector<int>& nums) {
        if (nums.size() <= 1) {
            return 0;
        }
        int step = 0;
        int cur_merge_max_index = 0, next_merge_max_index = 0;
        for (int i = 0; i < nums.size(); i++) {
            next_merge_max_index = max(nums[i] + i, next_merge_max_index);
            if (i == cur_merge_max_index) {
                if (cur_merge_max_index != nums.size() - 1) {
                    step++;
                    std::cout << "step: " << step << std::endl;
                    cur_merge_max_index = next_merge_max_index;
                    if (next_merge_max_index >= nums.size() - 1) break;
                }
            }
        }
        return step;
    }

    int jump(vector<int>& nums) {
        int steps = 0, end = 0, farthest = 0;
        for (int i = 0; i < nums.size() - 1; i++) {
            farthest = max(farthest, nums[i] + i);
            if (i == end) {
                steps++;
                std::cout << "steps: " << steps <<std::endl;
                end = farthest;
            }
        }
        return steps;
    }
};

int main(int argc, char const *argv[])
{
    std::vector<int> nums{2, 3, 0, 1, 4};
    std::shared_ptr<Solution> solution_ptr = std::make_shared<Solution>();
    solution_ptr->jump(nums);
    solution_ptr->_jump(nums); 
    return 0;
}
