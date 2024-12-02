#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Solution{
    public:
    int removeElementUseErase(vector<int>& nums, int val){
        // 调库一行代码就可以解决
        nums.erase(remove(nums.begin(), nums.end(), val), nums.end());
        // cout << "last index: " << remove(nums.begin(), nums.end(), val) << endl;
        return nums.size();

    }

    int removeElement(vector<int>& nums, int val){
        int slow = 0;
        for (int fast = 0; fast < nums.size(); fast++){
            if (nums[fast] != val){
                nums[slow] = nums[fast];
                slow++;
            }
        }
        return slow;
    }

    int removeElement2for(vector<int>& nums, int val){
        int size = nums.size();
        for (int i = 0; i < size; i++){
            if (nums[i] == val){
                for (int j = i + 1; j < size; j++)
                nums[j - 1] = nums[j];
                i--;
                size--;
            }
        }
        return size;
    }
};

int main(int argc, char *argv[])
{
    /* code */
    vector<int> nums = {2, 2, 2, 2, 5, 6, 7};
    Solution solution;
    // int index = solution.removeElementUseErase(nums, 2);
    int index = solution.removeElement2for(nums, 2);
    cout << "new size: " << nums.size() << endl;
    cout << "new size: " << index << endl;
    return 0;
}
