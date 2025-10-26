#include <deque>
#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        vector<int> res;
        if (nums.size() < k) {
            int max = nums[0];
            for (int num : nums) {
                max = std::max(num, max);
            }
            res.push_back(max);
            return res;
        }
        for (int i = 0; i < nums.size(); i++) {
            vector<int> tmp;
            if (i + 3 <= nums.size()) tmp = vector<int>(nums.begin() + i, nums.begin() + i + 3);
            else tmp = vector<int>(nums.begin() + i, nums.end());
            int max = nums[i];
            for (int x : tmp) {
                max = std::max(x, max);
            }
            res.push_back(max);
        }
        return res;
    }
};

class Solution1 {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        deque<int> dq;
        vector<int> ans;
        for (int i = 0; i < nums.size(); i++) {
            while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
            dq.push_back(i);
            if (dq.front() <= i - k) dq.pop_front();
            if (i >= k - 1) ans.push_back(nums[dq.front()]);
        }
        return ans;
    }
};

int main(int argc, char const *argv[])
{
    vector<int> nums{1,3,-1,-3,5,3,6,7};
    Solution s;
    vector<int> res = s.maxSlidingWindow(nums, 3);
    for (int x : res) {
        cout << x << " ";
    }
    cout << endl;
    return 0;
}