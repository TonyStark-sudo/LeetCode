#include<bits/stdc++.h>
using namespace std;
const int N = 10005, mod = 1000000007;
using ll = long long;
using pil = pair<int, ll>;
using pii = pair<int, int>;
int t, ans = 0;
int main() {
    int n, a, b, c;
    cin >> n >> a >> b >> c;
    int *nums = new int[n]{};
    // 读取砖块的颜色
    for (int i = 0; i < n; ++i) cin >> nums[i];
    vector<vector<int>> dp(n, vector<int> (n, 0));
    // 枚举区间的左端点
    for (int i = n - 1; i >= 0; --i) {
        dp[i][i] = a;
        // 枚举区间的右端点
        for (int j = i + 1; j < n; ++j) {
            // 单独消除这一块
            int r = max(dp[i + 1][j], dp[i][j - 1]) + a;
            // 将区间从这里划分成两块进行单独的消除
            for (int k = i + 1; k < j; ++k) {
                r = max(r, dp[i][k] + dp[k + 1][j]);
            }
            // 如果区间的左右端点颜色相同，可以把中间的全部消掉，再合并左右端点
            if (nums[i] == nums[j]) {
                // 两块一起消除
                r = max(r, dp[i + 1][j - 1] + b);
                // 如果区间中有一块砖块与左右端点的颜色相同，则把区间分为：
                // [i, k-1],[k+1,j]以及左右端点和中间这个相同颜色的砖块
                for (int k = i + 1; k < j; ++k) {
                    // 计算三消的得分
                    if (nums[i] == nums[k]) {
                        r = max(r, dp[i + 1][k - 1] + dp[k + 1][j - 1] + c);
                    }
                }
            }
            dp[i][j] = r;
        }
    }
    cout << dp[0][n - 1] << endl;
    return 0;
}