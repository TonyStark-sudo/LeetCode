#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
using namespace std;

// 返回最大利润的字符串表示（核心函数）
// 输入: prices 向量, k 最大交易次数
// 输出: string 表示最大利润
string maxProfitKStr(const vector<int>& prices, int k) {
    int n = prices.size();
    if (n == 0 || k == 0) return "0";
    // 如果 k >= n/2, 相当于无限次交易，贪心求和所有上升段
    if (k >= n/2) {
        long long profit = 0;
        for (int i = 1; i < n; ++i) if (prices[i] > prices[i-1]) profit += (prices[i] - prices[i-1]);
        return to_string(profit);
    }
    // 限制 k 不超过 n/2
    k = min(k, n/2);
    // dp[i][j][0/1] 表示在第 i 天（0-based）进行了 j 次交易（已完成的卖出次数），持有(1)/不持有(0) 的最大利润
    // 优化：只需要两层 j 与持有状态
    const long long NEG_INF = numeric_limits<long long>::min() / 4;
    vector<long long> hold(k+1, NEG_INF), unhold(k+1, NEG_INF);
    // 初始：尚未完成任何卖出，未持有为0
    unhold[0] = 0;
    // 第0天买入 -> hold[0] = -prices[0]
    hold[0] = -prices[0];
    for (int day = 1; day < n; ++day) {
        int price = prices[day];
        // 只更新可能的 j 范围：已经完成的交易次数不会超过 day
        int maxJCanHave = min(k-1, day-1); // 卖出次数 j 在 day 天之前最多为 day-1
        for (int j = maxJCanHave; j >= 0; --j) {
            // 卖出使完成的交易次数增加 1
            if (hold[j] != NEG_INF) unhold[j+1] = max(unhold[j+1], hold[j] + price);
        }
        // 更新 hold[j]，已完成 j 次卖出且在 day 天买入或保持持仓
        int maxJHold = min(k, day); // 持仓对应的已完成卖出次数不可能超过 day
        for (int j = 0; j <= maxJHold; ++j) {
            if (unhold[j] != NEG_INF) hold[j] = max(hold[j], unhold[j] - price);
        }
    }
    // 最终答案为最大 unhold[j] (所有完成的交易次数)
    long long ans = 0;
    for (int j = 0; j <= k; ++j) ans = max(ans, unhold[j]);
    return to_string(ans);
}

int main() {
    // 两个示例验证
    vector<int> a1 = {7,1,5,3,6,4};
    cout << "example1: " << maxProfitKStr(a1, 2) << " (expected 7)" << endl;
    vector<int> a2 = {8,6,5,3,3,2};
    cout << "example2: " << maxProfitKStr(a2, 2) << " (expected 0)" << endl;
    return 0;
}
