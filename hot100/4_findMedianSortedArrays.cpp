#include <vector>
#include <algorithm>
#include <iostream>

class Solution {
public:
    double findMedianSortedArrays(std::vector<int>& nums1, std::vector<int>& nums2) {
        double res;
        for (int i = 0; i < nums2.size(); i++) {
            nums1.push_back(nums2[i]);
        }
        for (auto num : nums1) {
            std::cout << num << " ";
        }
        std::sort(nums1.begin(), nums1.end());
        std::cout << "\n";
        for (auto num : nums1) {
            std::cout << num << " ";
        }
        std::cout << "\n";
        size_t size = nums1.size();
        if (size % 2 == 1) {
            res = nums1[size / 2];
        }
        else {
            res = (nums1[size / 2] + nums1[size / 2 - 1]) / 2.0;
        }
        return res;
    }
};

int main(int argc, char *argv[])
{
    std::vector<int> num1{1, 2};
    std::vector<int> num2{3, 4};
    Solution solution;
    double res = solution.findMedianSortedArrays(num1, num2);
    std::cout << "res = " << res << std::endl;
    /* code */
    return 0;
}
