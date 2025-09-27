// 有n个城市通过一些航班连接在一起。你将得到一个数组flights，其中flights[i] = [fromi, toi, pricei]表示该航班都从城市fromi开始，以价格pricei飞往toi。
// 现在给定所有的城市数量n，以及所有航班数组flights，和出发城市src，目的地dst，以及最多允许中转的次数k。
// 请你计算出从src到dst的最便宜价格，并且最多经过k次中转。如果没有这样的路线，则输出-1。
#include <iostream>
#include <vector>
#include <queue>
#include <climits>
using namespace std;

int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
	vector<vector<pair<int, int>>> graph(n);
	for (auto& f : flights) {
		graph[f[0]].push_back({f[1], f[2]});
	}
	// dp[i][j]: 从src到i，最多经过j次中转的最小花费
	vector<vector<int>> dp(n, vector<int>(k+2, INT_MAX));
	dp[src][0] = 0;
	for (int t = 1; t <= k+1; ++t) {
		for (int i = 0; i < n; ++i) {
			if (dp[i][t-1] == INT_MAX) continue;
			for (auto& e : graph[i]) {
				int v = e.first, w = e.second;
				if (dp[v][t] > dp[i][t-1] + w) {
					dp[v][t] = dp[i][t-1] + w;
				}
			}
		}
	}
	int ans = INT_MAX;
	for (int t = 1; t <= k+1; ++t) {
		ans = min(ans, dp[dst][t]);
	}
	return ans == INT_MAX ? -1 : ans;
}

int main() {
	int n, m;
	cin >> n >> m;
	vector<vector<int>> flights(m, vector<int>(3));
	for (int i = 0; i < m; ++i) {
		cin >> flights[i][0] >> flights[i][1] >> flights[i][2];
	}
	int src, dst, k;
	cin >> src >> dst >> k;
	int res = findCheapestPrice(n, flights, src, dst, k);
	cout << res << endl;
	return 0;
}