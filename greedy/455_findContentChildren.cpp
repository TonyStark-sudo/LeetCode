#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    int findContentChildren(vector<int>& g, vector<int>& s) {
        std::sort(g.begin(), g.end());
        std::sort(s.begin(), s.end());
        int index = s.size() - 1;
        int result = 0;
        for (int i = g.size() - 1; i >=0; i--) {
            if (g[i] <= s[index]) {
                index--;
                result++;
            }
        }
        return result;
    }
};