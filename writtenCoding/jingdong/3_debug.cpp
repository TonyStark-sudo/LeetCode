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
    
    // 调试输出
    cout << "商品序列: ";
    for (int i = 0; i < n; i++) {
        cout << goods[i] << " ";
    }
    cout << endl;
    
    cout << "需求: ";
    for (int i = 1; i <= m; i++) {
        cout << required[i] << " ";
    }
    cout << endl;
    
    // 检查是否所有商品都存在足够的数量
    unordered_map<int, int> totalCount;
    for (int i = 0; i < n; i++) {
        totalCount[goods[i]]++;
    }
    
    cout << "各商品总数: ";
    for (int i = 1; i <= m; i++) {
        cout << "商品" << i << ":" << totalCount[i] << " ";
    }
    cout << endl;
    
    // 检查是否可能存在满足条件的区间
    for (int i = 1; i <= m; i++) {
        if (totalCount[i] < required[i]) {
            cout << "商品" << i << "数量不足: " << totalCount[i] << " < " << required[i] << endl;
            cout << -1 << endl;
            return 0;
        }
    }
    
    // 使用滑动窗口找最短区间
    int left = 0, right = 0;
    int minLen = INT_MAX;
    unordered_map<int, int> windowCount;  // 当前窗口中每种商品的数量
    int satisfiedTypes = 0;  // 已经满足条件的商品种类数
    
    while (right < n) {
        // 扩展右边界
        int rightGoods = goods[right];
        windowCount[rightGoods]++;
        
        // 检查这种商品是否刚好满足要求
        if (windowCount[rightGoods] == required[rightGoods]) {
            satisfiedTypes++;
        }
        
        // 尝试收缩左边界
        while (satisfiedTypes == m) {
            // 更新最小长度
            minLen = min(minLen, right - left + 1);
            
            // 收缩左边界
            int leftGoods = goods[left];
            windowCount[leftGoods]--;
            
            // 检查收缩后是否还满足条件
            if (windowCount[leftGoods] < required[leftGoods]) {
                satisfiedTypes--;
            }
            left++;
        }
        
        right++;
    }
    
    if (minLen == INT_MAX) {
        cout << -1 << endl;
    } else {
        cout << minLen << endl;
    }
    
    return 0;
}
