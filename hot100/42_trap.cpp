#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    int trap(vector<int>& height) {
        int n = (int)height.size();
        if (n <= 2) return 0; // 少于3个柱子无法蓄水
        int left = 0, right = n - 1;
        int leftMax = 0, rightMax = 0;
        long long ans = 0; // 防止中间累加溢出（虽然题目范围一般不会）
        while (left < right) {
            if (height[left] < height[right]) {
                if (height[left] >= leftMax) {
                    leftMax = height[left];
                } else {
                    ans += leftMax - height[left];
                }
                ++left;
            } 
            else if(height[left] >= height[right]){
                if (height[right] >= rightMax) {
                    rightMax = height[right];
                } else {
                    ans += rightMax - height[right];
                }
                --right;
            }
        }
        return (int)ans;
    }
};

// 简单测试
int main() {
    vector<int> h {0,1,0,2,1,0,1,3,2,1,2,1};
    Solution s;
    cout << s.trap(h) << endl; // 期望 6
    return 0;
}