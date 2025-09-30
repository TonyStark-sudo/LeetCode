// 某超市要对一批货架上的商品进行备货检查，货架上共有n个连续的货位（编号1~n），每个货位放置一种商品，当前超市
// 有m种商品（编号1~m），其中第x类商品要求在检查区间内至少出现b次（b为非负整数）。请找出最短得连续货位区间[l,r], 使得在区间内
// 1到m类商品均至少出现b次。我们定义区间长度为r-l+1，如果不存在这样的区间，输出-1。
// 输入描述：
// 第一行包含两个整数n,m，分别表示货位数量和商品种类数。
// 第二行包含n个整数，依次表示每个货位上商品的商品编号。
// 第三行包含m个整数b1,b2,...,bm，其中bi表示第i类商品在区间内至少出现的次数。
// 输出描述：
// 输出一个整数，表示最短区间的长度，如果不存在这样的区间，输出-1。

#include <iostream>
#include <vector>
#include <unordered_map>
#include <climits>
using namespace std;

int main() {
    int n, m;
    cin >> n >> m;
    
    vector<int> goods(n);
    for (int i = 0; i < n; i++) {
        cin >> goods[i];
    }
    
    vector<int> required(m + 1);  // required[i] 表示第i种商品需要的最少次数
    for (int i = 1; i <= m; i++) {
        cin >> required[i];
    }
    
    // 检查是否所有商品都存在足够的数量
    vector<int> totalCount(m + 1, 0);
    for (int i = 0; i < n; i++) {
        if (goods[i] >= 1 && goods[i] <= m) {  // 只统计有效商品
            totalCount[goods[i]]++;
        }
    }
    
    // 检查是否可能存在满足条件的区间
    for (int i = 1; i <= m; i++) {
        if (totalCount[i] < required[i]) {
            cout << -1 << endl;
            return 0;
        }
    }
    
    // 使用滑动窗口找最短区间
    int left = 0;
    int minLen = INT_MAX;
    vector<int> windowCount(m + 1, 0);  // 当前窗口中每种商品的数量
    int satisfiedTypes = 0;  // 已经满足条件的商品种类数
    
    // 初始化满足条件的商品种类数（对于需求为0的商品）
    for (int i = 1; i <= m; i++) {
        if (required[i] == 0) {
            satisfiedTypes++;
        }
    }
    
    for (int right = 0; right < n; right++) {
        // 扩展右边界
        int rightGoods = goods[right];
        if (rightGoods >= 1 && rightGoods <= m) {  // 只处理有效商品
            windowCount[rightGoods]++;
            
            // 检查这种商品是否刚好满足要求
            if (windowCount[rightGoods] == required[rightGoods]) {
                satisfiedTypes++;
            }
        }
        
        // 尝试收缩左边界
        while (satisfiedTypes == m) {
            // 更新最小长度
            minLen = min(minLen, right - left + 1);
            
            // 收缩左边界
            int leftGoods = goods[left];
            if (leftGoods >= 1 && leftGoods <= m) {  // 只处理有效商品
                windowCount[leftGoods]--;
                
                // 检查收缩后是否还满足条件
                if (windowCount[leftGoods] < required[leftGoods]) {
                    satisfiedTypes--;
                }
            }
            left++;
        }
    }
    
    if (minLen == INT_MAX) {
        cout << -1 << endl;
    } else {
        cout << minLen << endl;
    }
    
    return 0;
}
