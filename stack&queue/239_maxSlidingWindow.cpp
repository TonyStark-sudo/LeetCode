#include <vector>
#include <algorithm>
#include <iostream>
#include <deque>

class _Solution {
public:
    std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k) {

        std::vector<int> ans;
        for (size_t i = k - 1; i < nums.size(); i++) {
            std::vector<int> tmp;
            int j = k;
            int l = i - k + 1;
            while (j--) {
                // std::cout << nums[l] << " ";
                tmp.push_back(nums[l++]);

            }
            // std::cout << std::endl;
            auto max_element = std::max_element(tmp.begin(), tmp.end());
            if (max_element != tmp.end()) {
                // std::cout << *max_element << " ";
                ans.push_back(*max_element);
            } 
            // std::cout << std::endl;
        }

        return ans;
    }
};

class Solution {
private:
    class Myqueue {
    public:
        std::deque<int> que;
        void pop(int value) {
            if (!que.empty() && que.front() == value) {
                que.pop_front();
            }
        }

        void push(int value) {

            while (!que.empty() && que.back() < value) {
            // if (!que.empty() && que.back() < value) {
                que.pop_back();

            }

            que.push_back(value);            
        }

        int front() {
            return que.front();
        }
    };

public:
    std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k) {
        Myqueue que;
        std::vector<int> result;
        for (int i = 0; i < k; i++) {
            que.push(nums[i]);
        }
        result.push_back(que.front());
        // 执行滑窗操作

        for (int i = k; i < nums.size(); i++) {
            que.pop(nums[i - k]);
            que.push(nums[i]);
            result.push_back(que.front());
        }
        return result;
    }
};

int main(int argc, char *argv[])
{
    std::vector<int> nums = {1, 3, -1, 5, 3, 6, 7};
    int k = 3;
    _Solution solution;
    std::vector<int> anws;
    anws = solution.maxSlidingWindow(nums, k);
    /* code */
    return 0;
}
