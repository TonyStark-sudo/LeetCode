// 小想有一个长度为 n 的全负数数组{a1,a2,...,an}，希望你构造一个长度为 n 的排列{b1,b2,...,bn}，
// 使得，a1 * b1 - a2 * b2 - ... - an * bn 的值尽可能大。
// 输入描述:
// 第一行输入整数T，表示测试数据的组数。
// 对于每组测试数据，第一行输入整数 n
// 第二行包含 n 个整数 a1,a2,...,an。
// 输出描述:
// 对于每组测试数据，新起一行输出一个整数，表示最大值。
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
	int T;
	cin >> T;
	while (T--) {
		int n;
		cin >> n;
		vector<int> a(n), b(n);
		for (int i = 0; i < n; ++i) cin >> a[i];
		for (int i = 0; i < n; ++i) b[i] = i + 1;
		sort(a.begin(), a.end(), greater<int>()); // a1最大，其余递减
		sort(b.begin(), b.end()); // b1最小，其余递增
		int res = a[0] * b[0]; // a1配b最小
		for (int i = 1; i < n; ++i) {
			res -= a[i] * b[n - i]; // 其余ai配b最大
		}
		// 修正：b[n-i]在i=1时是b[n-1]，i=n-1时是b[1]，但b[1]不是最大，b[n-1]才是最大
		// 应该用b[n-i]，但b数组是从0到n-1递增，b[n-1]最大
		// 直接写成b[n-i]没错，但建议写成b[n-i]，并且b数组初始化没问题
		// 但如果还是不对，建议直接输出调试
		// 其实最保险的写法如下：
		// a[0]配b[0]，a[1]配b[n-1]，a[2]配b[n-2]，...，a[i]配b[n-i]
		// 所以for循环应为：
		// for (int i = 1; i < n; ++i) res -= a[i] * b[n-i];
		// 但如果还是不对，建议直接输出调试
		cout << res << endl;
	}
	return 0;
}