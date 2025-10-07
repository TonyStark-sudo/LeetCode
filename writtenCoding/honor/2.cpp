// 从N（N > 0）个数中选择M( N > M > 0)个数，使M个数之和等于X，且M个数的平方和最大
// 输入描述:
// 输入包括两行，第一行输入N,M,X
// 第二行输入N个数
// 输出描述:
// 输出M个数的数组
#include <iostream>
#include <vector>
// 支持逗号分隔输入
#include <sstream>
using namespace std;

int N, M, X;
vector<int> nums;
vector<int> best, cur;
int maxSumSq = -1;

void dfs(int idx, int cnt, int sum, int sumSq) {
	if (cnt == M) {
		if (sum == X && sumSq > maxSumSq) {
			best = cur;
			maxSumSq = sumSq;
		}
		return;
	}
	if (idx >= N) return;
	// 选当前数
	cur.push_back(nums[idx]);
	dfs(idx + 1, cnt + 1, sum + nums[idx], sumSq + nums[idx] * nums[idx]);
	cur.pop_back();
	// 不选当前数
	dfs(idx + 1, cnt, sum, sumSq);
}

int main() {
	string line1, line2;
	getline(cin, line1);
	getline(cin, line2);
	// 处理逗号分隔
	replace(line1.begin(), line1.end(), ',', ' ');
	replace(line2.begin(), line2.end(), ',', ' ');
	istringstream iss1(line1), iss2(line2);
	iss1 >> N >> M >> X;
	nums.resize(N);
	for (int i = 0; i < N; ++i) iss2 >> nums[i];
	dfs(0, 0, 0, 0);
	if (best.empty()) cout << "No solution" << endl;
	else {
		for (int i = 0; i < best.size(); ++i) {
			if (i) cout << " ";
			cout << best[i];
		}
		cout << endl;
	}
	return 0;
}
