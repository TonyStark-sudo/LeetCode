#include <iostream>
// using namespace std;

class Solution {
public:
    bool isAnagram(std::string s, std::string t) {
        int i, hash[26] = {0};
        for (i = 0; i < s.size(); i++) {
            hash[s[i] - 'a']++;
        }
        for (i = 0; i < t.size(); i++) {
            hash[t[i] - 'a']--;
        }
        for (i = 0; i < 26; i++) {
            if (hash[i] != 0) return false;
        }

        return true;
    }
};