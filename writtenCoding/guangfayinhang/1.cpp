// 银行业务队列分配系统
// 银行有两类客户，普通客户（编号为奇数）和VIP客户（编号为偶数）。
// 你需要设计一个程序，将客户编号按照以下规则分配到两个业务队列中：
// 1. QueueA：存放普通客户（奇数编号），按输入顺序排列。
// 2. QueueB：存放VIP客户（偶数编号），按编号从大到小排序。
// 最终处理顺序：
// 1. 先处理VIP客户（QueueB）。
// 2. 每处理3个VIP客户后，必须处理1个普通客户（QueueA）。
// 3. 当其中一个队列为空时，处理剩余的客户。
// 最终输出：按上述规则处理后的客户编号序列。
/*
输入：一行整数，表示客户编号序列，以空格分隔。
输出：处理顺序的客户编号序列，以空格分隔，末尾无多余空格。
示例输入：
"8 2 1 3 9 4 11 13 15"
输出：
"8 4 2 1 3 9 11 13 15"
*/

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

// 返回处理后的结果字符串（核心可复用函数）
string processBankQueue(const string& line) {
    vector<int> odds, evens;
    istringstream iss(line);
    int x;
    while (iss >> x) {
        if (x % 2 == 0) evens.push_back(x);
        else odds.push_back(x);
    }
    sort(evens.begin(), evens.end(), greater<int>());
    size_t ei = 0, oi = 0;
    vector<int> res;

    // 当两队都有客户时，按每3个VIP插入1个普通客户的规则处理
    while (ei < evens.size() && oi < odds.size()) {
        for (int k = 0; k < 3 && ei < evens.size(); ++k) {
            res.push_back(evens[ei++]);
        }
        // 每处理3个（或处理到没有更多VIP）后，处理1个普通客户（如果有）
        if (oi < odds.size()) {
            res.push_back(odds[oi++]);
        }
    }

    // 若VIP队列还有剩余，全部输出（因为此时普通客户已空）
    while (ei < evens.size()) res.push_back(evens[ei++]);
    // 若普通队列还有剩余，全部按输入顺序输出
    while (oi < odds.size()) res.push_back(odds[oi++]);

    // 拼接成单个字符串返回
    ostringstream oss;
    for (size_t i = 0; i < res.size(); ++i) {
        if (i) oss << ' ';
        oss << res[i];
    }
    return oss.str();
}

// ...existing code...

int main() {
    string line;
    // 读取整行输入（可能包含多组空格分隔的整数）
    if (!std::getline(cin, line)) return 0;
    // 如果行为空或只有空白，直接退出
    bool hasDigit = false;
    for (char c : line) if (!isspace((unsigned char)c)) { hasDigit = true; break; }
    if (!hasDigit) return 0;
    // 使用新的返回字符串的核心函数并输出
    string out = processBankQueue(line);
    if (!out.empty()) cout << out << '\n';
    return 0;
}