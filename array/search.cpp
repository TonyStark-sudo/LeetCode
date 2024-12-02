#include <iostream>
#include <vector>

using namespace std;
class Solution {
    public:
    int search(vector<int>& nums, int target){
        int left = 0;
        int right = nums.size() - 1;
        while(left <= right) {
            int middle = left + (right - left) / 2;
            if (nums[middle] < target)
                left = middle + 1;
            else if (nums[middle] > target)
                right = middle - 1;
            else return middle;
        }
        return -1;
    } 
};

class Solution_1 {
    public:
    int search(vector<int>& nums, int target){
        int left = 0;
        int right = nums.size();
        while (left < right){
            int middle = left + (right - left) / 2;
            if (nums[middle] < target)
                left = middle + 1;
            else if (nums[middle] > target)
                right = middle;
            else return middle;
        }
        return -1; 
    }
};

int main(int argc, char *argv[])
{
    vector<int> nums = {1, 2, 3, 4, 5, 6, 7,88,9};
    int target = 88;
    int index;
    Solution solution;

    index = solution.search(nums, target);
    cout << "index is " << index << endl;

    return 0;
}
