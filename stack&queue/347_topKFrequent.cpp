#include <vector>
#include <unordered_map>
#include <queue>

using namespace std;

class Solution {
public:
    class mycomparison {
    public:
        bool operator() (const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) {
            return lhs.second > rhs.second;
        }
    };

    std::vector<int> topKFrequent(std::vector<int>& nums, int k) {
        std::unordered_map<int, int> map;
        for (int i = 0; i < nums.size(); i++) {
            map[nums[i]]++;
        }

        // 对元素的出现次数进行排序
        // 定义一个小顶堆， 大小为k
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, 
                            mycomparison> pri_que;

        // 用固定大小为k的小顶堆，扫面所有频率的数值
        for (unordered_map<int, int>::iterator it = map.begin(); it != map.end(); it++) {
            pri_que.push(*it);
            if (pri_que.size() > k) { // 如果堆的大小大于了K，则队列弹出，保证堆的大小一直为k
                pri_que.pop();
            }
        }

        // 找出前K个高频元素，因为小顶堆先弹出的是最小的，所以倒序来输出到数组
        vector<int> result(k);
        for (int i = k - 1; i >= 0; i--) {
            result[i] = pri_que.top().first;
            pri_que.pop();
        }
        return result;

    }
};