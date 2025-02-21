#include <string>
#include <iostream>

class Solution{
public:
    void _getNext(int* next, std::string s) {
        int j = -1;
        next[0] = j;
        for (size_t i = 1; i < s.size(); i++) {
            while (j > 0 && s[i] != s[j + 1]) {
                j = next[j - 1];
            }
            if (s[i] == s[j + 1]) {
                j++;
            }
            next[i] = j;
        }
    }

    void getNext(int* next, std::string s) {
        int j = -1;
        next[0] = j;
        for (size_t i = 1; i < s.size(); i++) {
            while (j >= 0 && s[i] != s[j + 1]) {
                j = next[j];
            }
            if (s[i] == s[j + 1]) {
                j++;
            }
            next[i] = j;
        }
    }

    bool repeatedSubstringPattern(std::string s) {
        int next[s.size()];
        getNext(next, s);
        size_t s_size = s.size();
        if (s_size % (s_size - next[s_size - 1]) == 0) return true;
        else return false;
    }
};

int main(int argc, char *argv[])
{
    std::string haystack = "abcabcabcabcabcabc";
    int next[haystack.size()];
    Solution solution;
    solution.getNext(next, haystack);
    std::cout << "next array:\n";
    for (int _ : next) {
        std::cout << _ << " ";
    }
    std::cout << "anw: " << solution.repeatedSubstringPattern(haystack) << std::endl;
    /* code */
    return 0;
}
