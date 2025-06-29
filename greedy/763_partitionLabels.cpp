#include <vector>
#include <string>

using namespace std;

class Solution {
public:
    vector<int> partitionLabels(string s) {
        vector<int> result;
        int hash[26] = {0};
        for (size_t i{0}; i < s.length(); i++) {
            hash[s[i] - 'a'] = i;
        }
        int left = 0, right = 0;
        for (size_t i{0}; i < s.length(); i++) {
            right = max(right, hash[s[i] - 'a']);
            if (i == right) {
                result.emplace_back(right - left + 1);
                left = right + 1;
            }
        }
        return result;
    }
};