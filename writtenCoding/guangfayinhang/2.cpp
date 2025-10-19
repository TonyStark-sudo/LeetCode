#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

class Solution {
public:
    vector<int> numberGame(vector<int>& N) {
        if (N.empty()) return {};

        int maxInput = 0;
        for (int x : N) if (x > maxInput) maxInput = x;

        // Choose an upper limit for preprocessing. Input constraint: N < 1e7.
        // Use at least 10,000,000 and add a buffer of 1,000,000, capped to 11,000,000.
        const int BASE = 10000000;
        const int CAP = 11000000;
        int LIMIT = max(BASE, maxInput + 1000000);
        if (LIMIT > CAP) LIMIT = CAP;

        vector<char> bad(LIMIT + 1, 0); // bad[i] == 1 means i is forbidden

        auto contains9 = [](int x)->bool{
            while (x) {
                if (x % 10 == 9) return true;
                x /= 10;
            }
            return false;
        };
        // Mark multiples of any number that contains '9' as bad.
        // We iterate s from 1..LIMIT and when s contains 9, mark its multiples.
        for (int s = 1; s <= LIMIT; ++s) {
            if (!contains9(s)) continue;
            for (int k = s; k <= LIMIT; k += s) bad[k] = 1;
        }

        // Build nextGood array: nextGood[i] = smallest j >= i such that bad[j]==0, or -1 if none
        vector<int> nextGood(LIMIT + 2, -1);
        int nxt = -1;
        for (int i = LIMIT; i >= 1; --i) {
            if (!bad[i]) nxt = i;
            nextGood[i] = nxt;
        }

        vector<int> ans; ans.reserve(N.size());
        for (int x : N) {
            if (x <= LIMIT && bad[x]) {
                ans.push_back(-1);
                continue;
            }
            // x is not forbidden (or beyond LIMIT, but inputs guarantee x < 1e7 so covered)
            int candidate = -1;
            if (x + 1 <= LIMIT) candidate = nextGood[x + 1];
            if (candidate == -1) {
                // Fallback linear search beyond LIMIT (rare). We'll check numbers one by one.
                int cur = max(x + 1, LIMIT + 1);
                for (;; ++cur) {
                    if (contains9(cur)) continue;
                    // check if cur divisible by any d that contains 9 (only need d <= sqrt(cur)?)
                    // Simpler: check divisors up to sqrt(cur) and their pair.
                    bool ok = true;
                    for (int d = 1; d * 1LL * d <= cur; ++d) {
                        if (cur % d != 0) continue;
                        int d2 = cur / d;
                        if (contains9(d) || contains9(d2)) { ok = false; break; }
                    }
                    if (ok) { candidate = cur; break; }
                    // safety guard (shouldn't run long)
                    if (cur - x > 2000000) { candidate = -1; break; }
                }
            }
            if (candidate == -1) ans.push_back(-1);
            else ans.push_back(candidate);
        }

        return ans;
    }
};

// 临时 main 用于快速测试
int main() {
    Solution sol;
    vector<int> input = {8,37,88};
    vector<int> out = sol.numberGame(input);
    for (size_t i = 0; i < out.size(); ++i) {
        if (i) cout << ",";
        cout << out[i];
    }
    cout << endl;
    return 0;
}