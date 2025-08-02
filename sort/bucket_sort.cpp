#include <iostream>
#include <vector>

using namespace std;

void selectSort(vector<int>& nums) {
    for (int i = 0; i < nums.size() - 1; i++) {
        for (int j = i + 1; j < nums.size(); j++) {
            if (nums[i] > nums[j]) {
                swap(nums[i], nums[j]);
            }
        }
    }
}

vector<int> bucketSort(vector<int>& nums) {
    vector<vector<int>> buckets(3);
    int bucketNum = 3;
    int min = nums[0], max = nums[0];
    for (int i = 0; i < nums.size(); i++) {
        min = std::min(min, nums[i]);
        max = std::max(max, nums[i]);
    }
    double bucketRange = (max - min) / bucketNum;
    for (int i = 0; i < nums.size(); i++) {
        int bucketIndex = static_cast<int>((nums[i] - min) / bucketRange);
        if (bucketIndex == bucketNum) bucketIndex--;
        buckets[bucketIndex].push_back(nums[i]);
    }

    for (auto& bucket : buckets) {
        selectSort(bucket);
    }
    vector<int> result;
    for (int i = 0; i < buckets.size(); i++) {
        for (int j = 0; j < buckets[i].size(); j++) {
            result.push_back(buckets[i][j]);
        }
    }
    return result;
}

int main(int argc, char const *argv[])
{
    vector<int> nums = {10, 3, 1, 8, 4, 7, 6};
    cout << "Before Sorted: ";
    for (int num : nums) {
        cout << num << " ";
    }
    cout << endl;
    
    vector<int> sorted = bucketSort(nums);
    cout << "After Sorted: ";
    for (int num : sorted) {
        cout << num << " ";
    }
    cout << endl;
    return 0;
}
