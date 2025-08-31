#include <vector>
#include <iostream>

using namespace std;

class MySolution {
public:
    void moveZeroes(vector<int>& nums) {
        int right = nums.size() - 1;
        for (int i = 0; i <= right; ) {
            if (nums[i] == 0 && right >= 0) {
                for (int j = i + 1; j <= right; j++) {
                    nums[j - 1] = nums[j];
                }
                nums[right] = 0;
                right--;
                for (int num : nums) {
                    cout << num << " ";
                }
                cout << endl;
            }
            else {
                i++;
            }
        }
    }
};

class Solution {
public: 
    void moveZeroes(vector<int>& nums) {
        int slow = 0;
        for (int fast = 0; fast < nums.size(); ++fast) {
            if (nums[fast] != 0) {
                if (slow != fast) {
                    swap(nums[slow], nums[fast]);
                }
                ++slow;
            }
            cout << "slow: " << slow << " fast: " << fast << " | ";
            for (int num : nums) {
                cout << num << " ";
            }            
            cout << endl;
        }
    }
};

int main(int argc, char const *argv[])
{
    vector<int> nums {1, 2, 0, 3, 4, 0, 5};
    Solution solution;
    solution.moveZeroes(nums);
    for (int num : nums) {
        cout << num << " ";
    }
    cout << endl;
    return 0;
}
