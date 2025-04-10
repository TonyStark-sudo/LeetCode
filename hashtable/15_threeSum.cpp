#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>

using namespace std;

class Solution {
    public:
    // wrong answer
    // vector<vector<int>> _threeSum(vector<int>& nums) {
    //     unordered_set<int> nums_set(nums.begin(), nums.end());
    //     vector<vector<int>> result;
    //     unordered_set<int, int, int> result_set;
    //     // unordered_set<vector<int>> 
    //     for (int i = 0; i < nums.size(); ++i) {
    //         for (int j = i + 1; j < nums.size(); ++j) {
    //             if (nums_set.find(0 - (nums[i] + nums[j])) != nums_set.end()) {
    //                 // result_set.insert({nums[i], nums[j], 0 - (nums[i] + nums[j])});
    //                 result.push_back({nums[i], nums[j], 0 - (nums[i] + nums[j])});
    //             }
    //         }
    //     }

    //     result_set = unordered_set<int, int, int>(result.begin(), result.end());
    //     return vector<vector<int>>(result_set.begin(), result_set.end());
    // }

    // // hash
    // vector<vector<int>> __threeSum(vector<int>& nums) {
    //     vector<vector<int>> result;
    //     sort(nums.begin(), nums.end());
    //     for (int i = 0; i < nums.size(); ++i) {
    //         if (nums[i] > 0 && i > 0) {
    //             break;
    //         }

    //         // 元素a去重
    //         if (nums[i] == nums[i - 1]) {
    //             continue;
    //         }
    //         unordered_set<int> set;
    //         for (int j = i + 1; j < nums.size(); ++j) {
    //             // 元素b去重
    //             if (j > i + 2 && nums[j] == nums[j - 1] && nums[j - 1] == nums[j - 2]) {
    //                 continue;
    //             }
    //             int c = 0 - (nums[i] + nums[j]);
    //             if (set.find(c) != set.end()) {
    //                 result.push_back({nums[i], nums[j], c});
    //                 // 三元组元素c去重
    //                 set.erase(c);
    //             }
    //             else set.insert(nums[j]);

    //         }
    //     }

    //     return result;
    // }

    vector<vector<int>> _threeSum(vector<int>& nums) {
        vector<vector<int>> result;
        sort(nums.begin(), nums.end());
        for (int i = 0; i < nums.size(); i++) {
            if (i > 0 && nums[i] == nums[i - 1]) break;
            int left = i + 1, right = nums.size() - 1;
            while (left < right) {
                if (nums[i] + nums[left] + nums[right] == 0) {
                    result.push_back({nums[i], nums[left], nums[right]});
                    left++;
                    right--;
                    while (left < right && nums[left] == nums[left + 1]) left++;
                    while (left < right && nums[right] == nums[right - 1]) right--; 
                }
                else if (nums[i] + nums[left] + nums[right] < 0) {
                    left++;
                }
                else if (nums[i] + nums[right] + nums[left] > 0) {
                    right--;
                }
                // left++;
                // right--;
            }
        }

        return result;
    }
    // 双指针
    vector<vector<int>> threeSum(vector<int>& nums) {
        vector<vector<int>> result;
        sort(nums.begin(), nums.end());
        for (int i = 0; i < nums.size(); ++i) {
            if (i > 0 && nums[i] == nums[i - 1]) continue;
            int left = i + 1;
            int right = nums.size() - 1;
            while (left < right) {
                if (nums[i] + nums[right] + nums[left] > 0) {
                    right--;
                }
                else if (nums[i] + nums[right] + nums[left] < 0) {
                    left++;
                }
                else {
                    result.push_back({nums[i], nums[left], nums[right]});
                    while (left < right && nums[left] == nums[left + 1]) left++;
                    while (left < right && nums[right] == nums[right - 1]) right--;

                    left++;
                    right--;    
                }
            }
        }

        return result;
    }


};

int main() {
    Solution sol;
    vector<int> nums = {-1, 0, 1, 2, -1, -4};
    vector<vector<int>> result = sol.threeSum(nums);
    
    for (const auto& triplet : result) {
        for (int num : triplet) {
            cout << num << " ";
        }
        cout << endl;
    }
    
    return 0;
}