#include <vector>
#include <iostream>

using namespace std;

void merge(vector<int>& nums, int start, int mid, int end) {
    vector<int> res;
    int cur_left = start, cur_right = mid + 1;
    while (cur_left <= mid && cur_right <= end) {
        if (nums[cur_left] <= nums[cur_right]) {
            res.push_back(nums[cur_left]);
            cur_left++;
        }
        else {
            res.push_back(nums[cur_right]);
            cur_right++;
        }
    }
    if (cur_left > mid) {
        for (int i = cur_right; i <= end; i++) {
            res.push_back(nums[i]);
        }
    }
    else {
        for (int i = cur_left; i <= mid; i++) {
            res.push_back(nums[i]);
        }
    }
    // 复制回原数组的指定范围
    for (int i = 0; i < res.size(); i++) {
        nums[start + i] = res[i];
    }
}

void mergeSort(vector<int>& nums, int start, int end) {
    if (start >= end) {
        return ;
    }
    int mid = start + (end - start) / 2;
    mergeSort(nums, start, mid);
    mergeSort(nums, mid + 1, end);
    merge(nums, start, mid, end);
}

void mergeSort(vector<int>& nums) {
    if (!nums.empty()) {
        mergeSort(nums, 0, nums.size() - 1);
    }
}

int main(int argc, char const *argv[])
{
    vector<int> nums{10, 3, 1, 8, 4, 7, 6};
    cout << "Before Sorted: [";
    for (int& num : nums) {
        cout << num << ", ";
    }
    cout << "]" << endl;
    mergeSort(nums);
    cout << "After Sorted: [";
    for (int& num : nums) {
        cout << num << ", ";
    }
    cout << "]" << endl;
    return 0;
}
