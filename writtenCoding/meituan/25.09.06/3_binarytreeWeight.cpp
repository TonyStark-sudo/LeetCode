
// 给定一棵树，根节点为1，集中第u个节点有点权au，定义f(k)为：选择树上k个互不相同的节点。你需要保证这k个节点两两不成祖先-子孙关系。f(k)为在所有可能的选择方案里最大的点权和。如果不存在任何一种合法的选择方案，则f(k)=-1。计算f(1), f(2), ..., f(n)。 [名词解释] 祖先节点：在一颗以u为根的树中，若点x在u到v的简单路径上，且x≠v, 则称x是v的祖先节点。根节点没有祖先节点。 输入描述：每个测试文件包含多组测试数据，第一行输入一个整数T（1≤T≤5000），代表树中的节点数量。第二行输入n个正整数a1,a2, ..., an（1≤ai≤10^9），表示每个节点的点权。接下来n-1行，第i行输入；两个正整数ui,vi（1≤ui,vi≤n）,表示树上一条从节点ui到vi的边，输入保证是一颗合法的树。除此之外，保证单个测试文件的n之和不超过5000。 输出描述：输出一行n个整数，其中第i个整数代表f(i)。 用C++实现代码，以ACM的形式呈现
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int N = 5010;
ll a[N];
vector<int> G[N];
ll dp[N][N]; // dp[u][k]: 以u为根的子树，选k个合法点的最大点权和
int sz[N];

void dfs(int u, int fa) {
    dp[u][0] = 0;
    dp[u][1] = a[u];
    sz[u] = 1;
    for (int v : G[u]) {
        if (v == fa) continue;
        dfs(v, u);
        for (int i = sz[u]; i >= 0; --i) {
            for (int j = 1; j <= sz[v]; ++j) {
                if (i + j > N - 1) continue;
                if (dp[u][i] != -1 && dp[v][j] != -1)
                    dp[u][i + j] = max(dp[u][i + j], dp[u][i] + dp[v][j]);
            }
        }
        sz[u] += sz[v];
    }
}

int main() {
    int n;
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    cin >> n;
    for (int i = 1; i <= n; ++i) cin >> a[i];
    for (int i = 1; i < n; ++i) {
        int u, v;
        cin >> u >> v;
        G[u].push_back(v);
        G[v].push_back(u);
    }
    memset(dp, -1, sizeof(dp));
    for (int i = 1; i <= n; ++i) dp[i][0] = 0;
    dfs(1, 0);
    for (int k = 1; k <= n; ++k) {
        ll ans = -1;
        for (int u = 1; u <= n; ++u) {
            if (dp[u][k] != -1) ans = max(ans, dp[u][k]);
        }
        cout << ans << (k == n ? '\n' : ' ');
    }
    return 0;
}
