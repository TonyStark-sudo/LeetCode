#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std;

class Solution {
    public:
    bool twoSum(vector<int>& nums, int target) {
        unordered_set<int> nums_set(nums.begin(), nums.end());
        for (int num : nums) {
            if (nums_set.find(target - num) != nums_set.end())
                return true;
        }
        return false;
    }

    vector<int> _twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> map;
        for (int i = 0; i < nums.size(); ++i) {
            auto iter = map.find(target - nums[i]);
            if (iter != map.end()) {
                return {iter->second, i};
            }
            map.insert(pair<int, int>(nums[i], i));
        }

        return {};
    }


};