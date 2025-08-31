#include <vector>
#include <unordered_set>

using namespace std;

class Solution {
public:
    int longestConsecutive(vector<int>& nums) {
        if (nums.empty()) return 0;
        unordered_set<int> nums_set(nums.begin(), nums.end());
        int ans = 0;
        for (int num : nums_set) {
            if (!nums_set.count(num - 1)) {
                int cur = num;
                int len = 1;
                while (nums_set.count(cur + 1)) {
                    ++cur;
                    ++len;
                }
                ans = max(ans, len);
            }
        }
        return ans;
    }
};

class MySolution {
public:
    int longestConsecutive(vector<int>& nums) {
        int length = 1;
        int tmp_length = 1;
        if (nums.size() <= 1) return nums.size();
        sort(nums.begin(), nums.end());
        for (int i = 1; i < nums.size(); ++i) {
            if (nums[i] == nums[i - 1]) continue;
            if (nums[i] - nums[i - 1] == 1) {
                tmp_length++;
            }
            else {
                length = max(tmp_length, length);
                tmp_length = 1;
            }
        }
        
        return max(tmp_length, length);
    }
};

int main(int argc, char const *argv[])
{
    
    return 0;
}
