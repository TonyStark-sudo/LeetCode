// 计算ID数量
// 小华刚刚参加了一个编译器课程，他想设计实现自己的编译器。首先，他设计了一种语言，他的语言最大支持N个字不同的字符，并且他规定了由
// 这些字符组成的ID，任何ID的长度需要大于等于1且等于L个字符，他希望设计一个程序知道他的语言总共能组成多少个ID。
// 例如，N=2，L=3，那么他的语言能组成的ID有：{0, 1, 00, 01, 10, 11, 000, 001, 010, 011, 100, 101, 110, 111}，一共14个ID。
// 由于ID的数量可能非常大，你只需要输出ID数量对1000000007取模的结果。
// 输入描述:
// 输入包含多个用例，每个用例将为包含两个整数N和L的一行。N是可以作为id的一部分的字符数，L是该语言支持的长度。当N=0，L=0时输入结束。（1<= N <= 65535，10 <= L <= 10^5）

// 输出描述:
// 对于每个用例，输出一行ID的总数
// ACM模式代码实现
#include <iostream>
using namespace std;

const int MOD = 1000000007;

// 快速幂
long long qpow(long long a, long long b) {
	long long res = 1;
	while (b) {
		if (b & 1) res = res * a % MOD;
		a = a * a % MOD;
		b >>= 1;
	}
	return res;
}

int main() {
	int N, L;
	while (cin >> N >> L) {
		if (N == 0 && L == 0) break;
		long long ans = 0;
		long long powN = 1;
		for (int i = 1; i <= L; ++i) {
			powN = powN * N % MOD;
			ans = (ans + powN) % MOD;
		}
		cout << ans << endl;
	}
	return 0;
}
