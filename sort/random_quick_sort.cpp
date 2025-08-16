#include <iostream>
#include <vector>
#include <random>

using namespace std;

random_device rd;
mt19937 gen(rd());

void printNums(const vector<int>& nums, const char* prefix = "") {
    cout << prefix << "[ ";
    for (int num : nums) {
        cout << num << ", ";
    }
    cout << "]" << endl;
}

int QuickSortPivot(vector<int>& nums, int start, int end) {
    int pivot = start;
    int j = start + 1;
    for (int i = start + 1; i <= end; i++) {
        if (nums[i] <= nums[pivot]) {
            swap(nums[i], nums[j]);
            j++;
        }
    }
    swap(nums[pivot], nums[j - 1]);
    pivot = j - 1;
    return pivot;
}

// 填坑法
int GetPivot(vector<int>& nums, int start, int end) {
    // int pivot = nums[start];
    uniform_int_distribution<int> dis(start, end);
    int randomIndex = dis(gen);
    swap(nums[start], nums[randomIndex]);

    int pivot = nums[start];
    int left = start, right = end;
    while (left < right) {
        // if (nums[right] < pivot) {
        //     nums[left] = nums[right];
        //     left++;
        // }
        // else if (nums[left] > pivot) {
        //     nums[right] = nums[left];
        //     right--;
        // }
        // else {
        //     left++;
        //     right--;
        // }
        while (left < right && nums[right] >= pivot) {
            right--;
        }
        if (left < right) {
            nums[left] = nums[right];
            left++;
        }
        while (left < right && nums[left] <= pivot) {
            left++;
        }
        if (left < right) {
            nums[right] = nums[left];
            right--;
        }
    }
    nums[left] = pivot;
    printNums(nums);
    return left;
}

void QuickSort(vector<int>& nums, int start, int end) {
    if (start >= end) {
        return ;
    }
    int pivot = GetPivot(nums, start, end);
    QuickSort(nums, start, pivot - 1);
    QuickSort(nums, pivot + 1, end);
}

int main(int argc, char const *argv[])
{
    vector<int> nums{10, 3, 1, 8, 4,  7, 6};
    printNums(nums, "Before Sorted: ");
    QuickSort(nums, 0, nums.size() - 1);
    printNums(nums, "After Sorted: ");
    return 0;
}