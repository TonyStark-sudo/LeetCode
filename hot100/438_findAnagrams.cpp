#include <string>
#include <vector>
#include <cstring>


using namespace std;

class Solution {
public:
    vector<int> findAnagrams(string s, string p) {
        vector<int> prefixIndex;
        if (p.length() > s.length()) return prefixIndex;
        
        int sCount[26] = {0};
        int pCount[26] = {0};

        for (int i = 0; i < p.length(); ++i) {
            sCount[s[i] - 'a']++;
            pCount[p[i] - 'a']++;
        }
        auto equalCount = [&]() {
            return memcmp(sCount, pCount, sizeof(sCount)) == 0;
        };
        if (equalCount()) prefixIndex.emplace_back(0);
        for (int i = 0; i < s.length() - p.length(); i++) {
            sCount[s[i] - 'a']--;
            sCount[s[i + p.size()] - 'a']++;
            if (equalCount()) {
                prefixIndex.emplace_back(i + 1);
            }
        }
        return prefixIndex;
    }
};