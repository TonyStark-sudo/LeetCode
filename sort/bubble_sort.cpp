#include <vector>
#include <iostream>
using namespace std;

void printVector(const vector<int>& nums) {
    for (const int& num : nums) {
        cout << num << " ";
    }
    cout << endl;
}

// 本质上是选择排序
void bubbleSort(vector<int>& nums) {
    for (size_t i{0UL}; i < nums.size(); ++i) {
        for (size_t j = i + 1; j < nums.size(); ++j) {
            if (nums[i] > nums[j]) {
                swap(nums[j], nums[i]);
            }
        }
        printVector(nums);
    }
}

void BubbleSort(vector<int>& nums) {
    for (size_t i{0UL}; i < nums.size() - 1; ++i) {
        for (size_t j = 0; j < nums.size() - 1 - i; ++j) {
            if (nums[j + 1] < nums[j]) {
                swap(nums[j], nums[j + 1]);
            }
        }
        printVector(nums);
    }
     
}

int main(int argc, char const *argv[])
{
    std::vector<int> nums{2, 3, 1, 5, 4, 7, 6};
    cout << "Original nums: ";
    printVector(nums);
    BubbleSort(nums);
    cout << "After Sorted: ";
    printVector(nums);
    return 0;
}
