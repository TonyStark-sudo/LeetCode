#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>

using namespace std;

class Solution {
public:
    vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {
        unordered_set<int> result;
        unordered_set<int> set_nums1(nums1.begin(), nums1.end());
        for (int num : nums2) {
            // 如果没有找到该元素，find 会返回 unordered_set::end()，即该集合的结束迭代器
            if (set_nums1.find(num) != set_nums1.end()) {
                result.insert(num);
            }
        }

        return vector<int>(result.begin(), result.end());  
    }


    std::vector<int> _intersection(vector<int>& nums1, vector<int>& nums2) {
        vector<int> result;
        for (int i = 0; i < nums1.size(); i++) {
            for (int j = 0; j < nums2.size(); j++) {
                if (nums2[j] == nums1[i]) {
                    result.push_back(nums1[i]);
                }
            }
        }
        // set这种数据结构，带有去重操作  
        set<int> uniqueSet(result.begin(), result.end());
        vector<int> _result(uniqueSet.begin(), uniqueSet.end()); 
        return _result;
    }


};