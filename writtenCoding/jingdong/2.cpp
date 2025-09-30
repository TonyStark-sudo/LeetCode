// 美丽的三元组
// 小Q得到了一个包含n个整数的数组a。他非常喜欢三元组，所以他把所有相邻的三个数看作一个三元组，然后抄到了他的本子上，一共有n-2个三元组。
// 当且仅当两个三元组恰好有一个位置的数字不同时，这两个三元组才被称为相似的。即满足以下条件之一：
// (a1,b1,c1)和(a2,b2,c2)是相似的，当且仅当
// a1=a2,b1=b2,c1!=c2
// a1=a2,b1!=b2,c1=c2
// a1!=a2,b1=b2,c1=c2
// 小Q想知道他的本子上有多少对相似的三元组，你能帮帮他吗？
// 输入描述：单租数据包含多组测试数据。
// 第一行包含一个整数T(1<=T<=100)，表示测试数据的组数。
// 对于每组测试数据：
// 第一行包含一个整数n(3<=n<=10^5)，表示数组a的长度。
// 第二行包含n个整数a1,a2,...,an(1<=ai<=10^9)，表示数组a的内容。
// 输出描述：对于每组测试数据，输出一个整数，表示相似三元组的对数。

#include <iostream>
#include <vector>
#include <map>
using namespace std;

struct Triplet {
    int a, b, c;
    Triplet(int x, int y, int z) : a(x), b(y), c(z) {}
};

// 判断两个三元组是否相似
bool isSimilar(const Triplet& t1, const Triplet& t2) {
    int diffCount = 0;
    if (t1.a != t2.a) diffCount++;
    if (t1.b != t2.b) diffCount++;
    if (t1.c != t2.c) diffCount++;
    
    return diffCount == 1;  // 恰好有一个位置不同
}

int main() {
    int T;
    cin >> T;
    
    while (T--) {
        int n;
        cin >> n;
        vector<int> a(n);
        
        for (int i = 0; i < n; i++) {
            cin >> a[i];
        }
        
        // 生成所有三元组
        vector<Triplet> triplets;
        for (int i = 0; i < n - 2; i++) {
            triplets.push_back(Triplet(a[i], a[i + 1], a[i + 2]));
        }
        
        // 统计相似三元组对数
        long long count = 0;
        int m = triplets.size();
        
        // 使用map来优化，按照模式分类
        // 对于模式 (a,b,*)，统计有多少个不同的c值
        map<pair<int, int>, map<int, int>> pattern_ab; // (a,b) -> {c -> count}
        map<pair<int, int>, map<int, int>> pattern_ac; // (a,c) -> {b -> count}
        map<pair<int, int>, map<int, int>> pattern_bc; // (b,c) -> {a -> count}
        
        // 统计每种模式
        for (const auto& t : triplets) {
            pattern_ab[{t.a, t.b}][t.c]++;
            pattern_ac[{t.a, t.c}][t.b]++;
            pattern_bc[{t.b, t.c}][t.a]++;
        }
        
        // 计算相似对数
        // 模式1: a1=a2, b1=b2, c1!=c2
        for (const auto& p : pattern_ab) {
            long long total = 0;
            for (const auto& c : p.second) {
                total += c.second;
            }
            // 总对数 - 相同的对数
            long long samePairs = 0;
            for (const auto& c : p.second) {
                samePairs += (long long)c.second * (c.second - 1) / 2;
            }
            count += total * (total - 1) / 2 - samePairs;
        }
        
        // 模式2: a1=a2, b1!=b2, c1=c2
        for (const auto& p : pattern_ac) {
            long long total = 0;
            for (const auto& b : p.second) {
                total += b.second;
            }
            long long samePairs = 0;
            for (const auto& b : p.second) {
                samePairs += (long long)b.second * (b.second - 1) / 2;
            }
            count += total * (total - 1) / 2 - samePairs;
        }
        
        // 模式3: a1!=a2, b1=b2, c1=c2
        for (const auto& p : pattern_bc) {
            long long total = 0;
            for (const auto& a : p.second) {
                total += a.second;
            }
            long long samePairs = 0;
            for (const auto& a : p.second) {
                samePairs += (long long)a.second * (a.second - 1) / 2;
            }
            count += total * (total - 1) / 2 - samePairs;
        }
        
        cout << count << endl;
    }
    
    return 0;
}