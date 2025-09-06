// 小美有一颗n个节点（编号为1~n）、由n-1条边构成的无向树，根节点为1，每个节点i有一个值ai，对于任意节点u，其权值定义如下：如果u有子节点，则权值为该节点与所有子节点值的按位异或的最大值，即max au xor av；如果u是叶子节点，则权值为au。接下来小美会对这棵树进行m次操作，每次操作从以下三种类型中选一种执行：类型1 x y: 将节点x的值ax修改为y, (1≤x≤n, 1≤y≤10^9）；类型2 x y: 查询当前所有编号在区间[x,y]内的节点的最大权值，（1≤x≤y≤n)；类型3 x y: 查询以节点x为根的子树中所有节点的最大权值（1≤x≤n) 输入描述：第一行输入两个整数n,m（1≤n, m≤3*10^5），分别表示节点数和操作次数；第二行输入n个整数a1, a2, ..., an（1≤ai≤10^9）,表示初始时每个节点的值。接下来n-1行，第i行输入两个整数ui和vi（1≤ui, vi≤n; ui≠vi ）, 表示树上第i条数边，随后m行，每行对应一次操作，格式如题目描述所示。 
// 输出描述：对于每个查询操作（类型2或3），输出一行，表示查询结果（最大权值）。 用C++代码实现，以ACM的形式呈现
#include <bits/stdc++.h>
using namespace std;
const int N = 3e5 + 5;
vector<int> g[N];
int a[N], n, m;
int in[N], out[N], id[N], clk = 0, fa[N];
int mx[N * 4];

void dfs(int u, int f) {
	in[u] = ++clk;
	id[clk] = u;
	fa[u] = f;
	for (int v : g[u]) if (v != f) dfs(v, u);
	out[u] = clk;
}

int calc(int u) {
	int res = a[u];
	for (int v : g[u]) if (v != fa[u]) res = max(res, a[u] ^ a[v]);
	return res;
}

void build(int o, int l, int r) {
	if (l == r) {
		mx[o] = calc(id[l]);
		return;
	}
	int mid = (l + r) / 2;
	build(o * 2, l, mid);
	build(o * 2 + 1, mid + 1, r);
	mx[o] = max(mx[o * 2], mx[o * 2 + 1]);
}

void update(int o, int l, int r, int pos) {
	if (l == r) {
		mx[o] = calc(id[l]);
		return;
	}
	int mid = (l + r) / 2;
	if (pos <= mid) update(o * 2, l, mid, pos);
	else update(o * 2 + 1, mid + 1, r, pos);
	mx[o] = max(mx[o * 2], mx[o * 2 + 1]);
}

int query(int o, int l, int r, int ql, int qr) {
	if (ql > r || qr < l) return 0;
	if (ql <= l && r <= qr) return mx[o];
	int mid = (l + r) / 2;
	return max(query(o * 2, l, mid, ql, qr), query(o * 2 + 1, mid + 1, r, ql, qr));
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);
	cin >> n >> m;
	for (int i = 1; i <= n; ++i) cin >> a[i];
	for (int i = 1; i < n; ++i) {
		int u, v;
		cin >> u >> v;
		g[u].push_back(v);
		g[v].push_back(u);
	}
	dfs(1, 0);
	build(1, 1, n);
	while (m--) {
		int op, x, y;
		cin >> op >> x >> y;
		if (op == 1) {
			a[x] = y;
			update(1, 1, n, in[x]);
			for (int v : g[x]) if (v != fa[x]) update(1, 1, n, in[v]);
			if (fa[x]) update(1, 1, n, in[fa[x]]);
		} else if (op == 2) {
			cout << query(1, 1, n, x, y) << '\n';
		} else {
			cout << query(1, 1, n, in[x], out[x]) << '\n';
		}
	}
	return 0;
}
