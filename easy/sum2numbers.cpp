#include <iostream>
#include <vector>

using namespace std;

class Solution {
    public:
    vector<int> twoSum(vector<int>& nums, int target) {
        int n = nums.size();
        for(int i = 0; i < n; i++)
            for(int j = i + 1; j < n; j++)
                if(nums[i] + nums[j] == target)
                    return {i, j};
                return {};
    }
};

int main(int argc, char *argv[])
{
    vector<int> nums = {1, 4, 5, 6};
    int target = 7;
    Solution solution;
    vector<int> anw = solution.twoSum(nums, target);
    for(int i = 0; i < anw.size(); i++)
        cout << "index of the nums: " << anw[i] << endl;
    return 0;
}

