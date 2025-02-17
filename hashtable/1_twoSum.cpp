#include <vector>
#include <unordered_map>

class Solution {
    public:
    std::vector<int> _twoSum(std::vector<int>& nums, int target) {
        for (int i = 0; i < nums.size(); i++) {
            for (int j = i + 1; j < nums.size(); j++) {
                if (nums[i] + nums[j] == target) {
                    return {i, j};
                }
            }
        }

        return {};
    }

    // use hashtable
    std::vector<int> twoSum(std::vector<int>& nums, int target) {
        std::unordered_map<int, int> map;
        for (int i = 0; i < nums.size(); i++) {
            // 遍历当前元素， 并在map中寻找是否有匹配的key
            auto iter = map.find(target - nums[i]);
            if (iter != map.end()) {
                return {iter->second, i};
            }

            map.insert(std::pair<int, int>(nums[i], i));
        }

        return {};
    }     
};