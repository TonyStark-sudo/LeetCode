#include <string>
#include <vector>
#include <iostream>

class _Solution {
public:
    bool _repeatedSubstringPattern(std::string s) {
        std::vector<std::string> substrings;
        
        int j = 0;
        int s_size = s.size();
        while (s_size--) {
            std::string temp = "";
            for (int i = j; i < s.size(); i++) {
                temp.push_back(s[i]);
                substrings.push_back(temp);
            }
            // substrings.push_back(temp);
            j++;   
        }

        for (auto substr : substrings) {

            while(substr.size() <= s.size()) {
                substr.append(substr);
                if (substr == s) return true;
            }

            // std::cout << substr << " ";              
        }

        // std::cout << "\nsubstrings size: " << substrings.size() << std::endl;
        return false;
    }
};
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
    std::string  s = "abcab";
    Solution solution;
    std::cout << "ans is: " << solution.repeatedSubstringPattern(s) << std::endl;
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
