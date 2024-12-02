#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
class Solution{
    public:
    int sum_all = 0;
    int result = INT32_MAX;
    int len = 0;
    int _minSubArrayLen(vector<int>& nums, int target){
        // 误解题意了！！！
        sort(nums.begin(), nums.end());
        int sum = 0;
        for (int i = nums.size() - 1; i >= 0; i--){
            sum += nums[i]; 
            if(sum >= target)
                return nums.size() - i;
        }
        return 0;
    }

    int minSubArrayLen(vector<int>& nums, int target){

        for (int i = 0; i < nums.size(); i++){
            sum_all = 0;
            for (int j = i; j < nums.size(); j++){
                sum_all +=nums[j];
                cout << "sum_all is : " << sum_all << endl;
    
                if (sum_all >= target){
                    len = j - i + 1;
                    result = result < len ? result : len;
                    break;
                }
            }
        }
        return result == INT32_MAX ? 0 : result;
    }

    int minSubArrayLenSlideWind(vector<int>& nums, int target){
        int i = 0;
        for (int j = 0; j < nums.size(); j++){
            sum_all += nums[j];
            while (sum_all >= target){
                len = j - i + 1;
                result = result < len ? result : len;
                sum_all -= nums[i++]; // 这里体现滑动窗口的精髓， 不断更新i（子序列的起始位置）
            }
        }

        return result == INT32_MAX ? 0 : result;
    }
};

int main(int argc, char *argv[])
{
    vector<int> nums = {2, 3, 1, 2, 4, 3};
    Solution solution;
    int len = solution.minSubArrayLenSlideWind(nums, 7);
    cout << "lenth is: " << len << endl; 
    return 0;
}
