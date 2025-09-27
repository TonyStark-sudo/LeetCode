// 使用ACM代码模式，用C++实现
// 给定你一个一维数组prices，prices[i]是某支股票第i天的价格。你只能选择在某一天买入这只股票，并选择在未来的某一个不同的日子卖出该股票。
// 设计一个算法来计算你所能获取的最大利润，且只能买卖交易一次。返回你从这比交易中获得的最大利润， 如果无法获取利润，返回0。
#include <iostream>
#include <vector>
using namespace std;

int maxProfit(vector<int>& prices) {
	int minPrice = INT_MAX, maxP = 0;
	for (int p : prices) {
		if (p < minPrice) minPrice = p;
		else if (p - minPrice > maxP) maxP = p - minPrice;
	}
	return maxP;
}

int main() {
	int n;
	cin >> n;
	vector<int> prices(n);
	for (int i = 0; i < n; ++i) cin >> prices[i];
	cout << maxProfit(prices) << endl;
	return 0;
}