#include <vector>
#include <memory>
#include <iostream>

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
            std::cout << "sum = " << sum << ", result = " << result << std::endl;
            if (sum < 0) sum = 0;
        }
        return result;
    }
};

int main(int argc, char const *argv[])
{
    std::vector<int> nums{-2, 1, -3, 4, -1, 2, 1, -5, 4};
    std::shared_ptr<Solution> solution_ptr = std::make_shared<Solution>();
    solution_ptr->maxSubArray(nums);    
    return 0;
}
