#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
class Solution{
    public:
    vector<int> sortedSquare(vector<int>& nums){
        for (int i = 0; i < nums.size(); i++){
            nums[i] = nums[i] * nums[i];
        }
        sort(nums.begin(), nums.end());
        return nums; 
    } 

    vector<int> sortedsquare2p(vector<int>& nums){
        vector<int> result(nums.size(), 0);
        int i = 0, j = nums.size() - 1, k = nums.size() - 1;
        while(i <= j){
            if (nums[i] * nums[i] >= nums[j] * nums[j]){
                result[k--] = nums[i] * nums[i];
                i++;
            }
            else {
                result[k--] = nums[j] * nums[j];
                j--;
            }
        }
        return result;
    }
};

int main(int argc, char *argv[])
{
    vector<int> nums = {-3, -1, -1, 4, 2}, ans;
    Solution solution;
    // ans = solution.sortedSquare(nums);
    ans = solution.sortedsquare2p(nums);
    for (int num : nums){
        cout << num << " ";
    }
    cout << endl; 
    for (int num : ans){
        cout << num << " ";
    }
    /* code */
    return 0;
}
