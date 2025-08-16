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

void shellSort(vector<int>& nums) {
    int delta = nums.size() / 2;
    while (delta != 0) {
        for (int i = delta; i < nums.size(); i++) {
            int j = i;
            int tmp = nums[i];
            while (j >= delta && tmp < nums[j - delta]) {
                nums[j] = nums[j - delta];
                j -= delta;
            }
            nums[j] = tmp;
            printNums(nums);
        }
        delta = delta / 2;
        printNums(nums, "after bound: ");
    }
}

int main(int argc, char const *argv[])
{
    vector<int> nums{7, 6, 5, 4, 3, 2, 1};
    shellSort(nums);    
    return 0;
}
