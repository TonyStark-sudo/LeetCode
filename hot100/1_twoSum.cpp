#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>

using namespace std;
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> umap;
        for (int i = 0; i < nums.size(); i++) {
            if (umap.find(target - nums[i]) != umap.end()) {
                return {umap.find(target - nums[i])->first, i};
            }
            umap.insert({nums[i], i});
        }
        return {};
    }
};


int main() {
    std::vector<int> vec = {10, 20, 30, 40, 50};
    std::unordered_map<int, int> umap;

    for (size_t i = 0; i < vec.size(); i++) {
        umap[i] = vec[i];  // 使用索引作为 key
    }

    // 遍历 unordered_map
    for (const auto& pair : umap) {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }

    return 0;
}
