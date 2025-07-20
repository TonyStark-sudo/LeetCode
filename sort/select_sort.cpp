#include <iostream>
#include <vector>
using namespace std;

void printVector(const vector<int>& nums) {
    for (const int& num : nums) {
        cout << num << " ";
    }
    cout << endl;
}

void selectSort(vector<int>& nums) {
    for (size_t i{0UL}; i < nums.size() - 1; i++) {
        int min_index = i;
        for (size_t j = i + 1; j < nums.size(); j++) {
            if (nums[j] < nums[min_index]) {
                min_index = j;
            }
        }
        swap(nums[i], nums[min_index]);
        printVector(nums);
    }
}

int main(int argc, char const *argv[])
{
    std::vector<int> nums{2, 3, 1, 5, 4, 7, 6};
    cout << "Original nums: ";
    printVector(nums);
    selectSort(nums);
    cout << "After Sorted: ";
    printVector(nums);
    return 0;
}
