#include <iostream>
#include <vector>

using namespace std;
void insertSort(vector<int>& nums) {
    for (int i = 1; i < nums.size(); i++) {
        int cur = nums[i];
        int j = i - 1;
        while (j >= 0 && nums[j] > cur) {
            nums[j + 1] = nums[j];
            j--;
        }
        nums[j + 1] = cur;
        cout << "bound " << i << ": ";
        for (int& num : nums) {
            cout << num << ", ";
        }
        cout << endl;        
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
    insertSort(nums);
    cout << "After Sorted: [";
    for (int& num : nums) {
        cout << num << ", ";
    }
    cout << "]" << endl;
    return 0;
}
