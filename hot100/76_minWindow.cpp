#include <string>
using namespace std;
class Solution {
public:
    string minWindow(string s, string t) {
        if(t.empty() || s.size() < t.size()) return "";
        int need[128] = {0};
        for (char c : t) {
            need[(unsigned char)c]++;
        }
        int required = t.size();
        int left = 0, bestLen = INT32_MAX, bestStart = 0;
        for (int right = 0; right < s.size(); right++) {
            unsigned char rc = s[right];
            // if (need[rc]-- > 0) {
            //     required--;
            // }
            if (need[rc] > 0) {
                need[rc]--;
                required--;
            }
            else {
                need[rc]--;
            }
            while (required == 0) {
                int curLen = right - left + 1;
                if (curLen < bestLen) {
                    bestLen = curLen;
                    bestStart = left;
                }
                unsigned char lc = s[left++];
                if (++need[lc] > 0) {
                    required++;
                }
            }
        }
        return bestLen == INT32_MAX ? "" : s.substr(bestStart, bestLen);
    }
};