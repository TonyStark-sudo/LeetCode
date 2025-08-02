#include <vector>
#include <iostream>

using namespace std;

void countingSort(vector<int>& nums) {
    int max = *max_element(nums.begin(), nums.end());
    vector<int> counter(max + 1);
    for (int i = 0; i < nums.size(); i++) {
        counter[nums[i]]++;
    }
    vector<int> result(nums.size());
    nums = {};
    for (int i = 0; i < counter.size(); i++) {
        while(static_cast<bool>(counter[i]--)) {
            nums.push_back(i);
        }
    }
}

int main(int argc, char const *argv[])
{
    vector<int> nums = {10, 3, 1, 8, 4, 7, 6};
    cout << "Before Sorted: ";
    for (int num : nums) {
        cout << num << " ";
    }
    cout << endl;
    
    countingSort(nums);
    cout << "After Sorted: ";
    for (int num : nums) {
        cout << num << " ";
    }
    cout << endl;
    return 0;
}
