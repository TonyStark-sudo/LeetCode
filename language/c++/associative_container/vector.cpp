#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_set>

using namespace std;

/*
vector容器的基本使用
题目：
给你一组整数，删除所有重复元素，并保持原顺序输出。
考点：
vector的顺序访问、动态扩容、erase操作。
提示：
可以用std::find或者std::unordered_set来辅助判断重复元素。
*/

class Solution {
public:
    vector<int> deleteDumpEle(const vector<int>& nums) {
        vector<int> deleted;
        for (int num : nums) {
            if (find(deleted.begin(), deleted.end(), num) == deleted.end()) {
                deleted.push_back(num);
            }
        }
        for (int x : deleted) {
            cout << x << " ";
        }
        return deleted;
    }
};

class Solution1 {
public:
    vector<int> deleteDumpEle(const vector<int>& nums) {
        unordered_set<int> set(nums.begin(), nums.end());
        return vector<int>(set.begin(), set.end());
    }
};

int main(int argc, char const *argv[])
{
    // 初始化vector容器
    vector<int> vec{1, 2, 3, 4, 5};
    // vector<int> nums(12, 0);
    vector<int> v1 = {1, 2, 3, 4, 5, 6};

    // C++数组的初始化方式
    int a[6] = {1, 2, 3, 4, 5, 6};
    // std::cout << "vec[6] = " << vec[6] << std::endl;
    // 异常
    // std::cout << "vec.at(6) = " << vec.at(6) << std::endl;
    std::cout << "vec size = " << vec.size() << std::endl;
    std::cout << "vec capa = " << vec.capacity() << std::endl;
    vector<int> nums{1, 2, 2, 2, 3, 4, 4, 5};
    Solution1 s1;
    Solution s;
    vector<int> res = s1.deleteDumpEle(nums);
    for (int x : res) {
        cout << x << " ";
    }
    cout << endl;    
    s.deleteDumpEle(nums);
    cout << endl;
    return 0;
}
