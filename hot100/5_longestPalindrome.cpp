#include <string>
#include <vector>
#include <list>
#include <deque>
#include <algorithm>
#include <map>

using namespace std;

class Solution {
public:
    bool isPalindrome(string& s) {
        string inverse_s;
        for (int i = s.size() - 1; i >= 0; i--) {
            inverse_s.push_back(s[i]);
        }
        if (s == inverse_s) return true;
        else return false;
    }

    string longestPalindrome(string s) {
        string substring;
        int length = 0;
        map<int, string> map_tmp;
        vector<int> vec;
        for (int i = 0; i < s.size(); i++) {
            for (int j = i + 1; j < s.size(); j++) {
                // auto iter_left = &s[i];
                // auto iter_right = &s[j];
                string tmp(s.begin() + i, s.begin() + j + 1);
                // string tmp = s.substr(i, j - i + 1);
                if(isPalindrome(tmp)) {
                    int size = tmp.size();
                    map_tmp.insert({size, tmp});
                    length = max(length, size);
                }
            }
        }
        return map_tmp[length];

    }
};