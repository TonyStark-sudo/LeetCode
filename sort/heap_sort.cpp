#include <vector>
#include <iostream>

using namespace std;

void printNums(const vector<int>& nums, const char* prefix = "") {
    cout << prefix << "[ ";
    for (int num : nums) {
        cout << num << ", ";
    }
    cout << "]" << endl;
}

void maxHeapify(vector<int>& nums, int start, int end) {
    int parent = start;
    int son = parent * 2;
    while (son <= end) {
        if (son + 1 <= end && nums[son + 1] > nums[son]) {
            son++;
        }
        if (nums[parent] < nums[son]) {
            swap(nums[parent], nums[son]);
            parent = son;
            son = son * 2;
        }
        else
        {
            break;
        }
    }
}

void heapSort(vector<int>& nums) {
    nums.insert(nums.begin(), int{});
    int nums_size = nums.size() - 1;
    printNums(nums, "original nums: ");
    for (int i = nums_size / 2; i >= 1; i--) {
        maxHeapify(nums, i, nums_size);
    }
    printNums(nums, "build heap: ");
    for (int i = nums_size; i >= 1; i--) {
        swap(nums[1], nums[i]);
        maxHeapify(nums, 1, i - 1);
    }
    nums.erase(nums.begin());
    printNums(nums, "sorted nums: ");
}

int main(int argc, char const *argv[])
{
    vector<int> nums{1, 2, 3, 4, 5, 6, 7};
    heapSort(nums);
    return 0;
}
