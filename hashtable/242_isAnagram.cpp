#include <iostream>
<<<<<<< HEAD
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
=======

using namespace std;

class Solution {
    bool isAnagram(string s, string t) [
        
    ]
>>>>>>> 7c6ba34a94b86fc79e9ff055cce4d10ff153cc2c
};