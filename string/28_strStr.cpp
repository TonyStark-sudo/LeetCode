#include <iostream>
#include <string>

using namespace std;
class Solution {
public:
    // two for
    // int _strStr(string haystack, string needle) {
    //     size_t h_size = haystack.size();
    //     size_t n_size = needle.size();

    //     if (n_size == 0) return 0;
    //     for (int i = 0; i <= h_size - n_size; i++) {
    //         int j = 0;
    //         for (; j < n_size; j++) {
    //             if (needle[j] != haystack[j + i]) {
    //                 break;
    //             }
    //         }

    //         if (j == n_size) {
    //             return i;
    //         } 
    //     }

    //     return -1;
    // }

    // next array
    int strStr(string haystack, string needle) {
        
        int j  = 0;
        if (needle.size() == 0) return 0;
        
        int* next;
        getnext(next, needle);
        for (size_t i = 0; i < haystack.size(); i++) {
            while (haystack[i] != needle[j]) {
                j = next[i - 1];
            }
            if (haystack[i] == needle[j]) {
                j++;
            }
            if (j == needle.size() - 1) {
                return (i - needle.size() + 1);
            }
        }

        return -1;
    }
    // needle[i] != needle[j]时j需要回退
    void getnext(int* next, string s) {
        int j = 0;
        next[0] = j;

        for (int i = 1; i < s.size(); i++) {
            while (j > 0 && s[i] !=  s[j]) {
                j = next[j - 1];
            }

            if (s[i] == s[j]) {
                j++;
            }
            next[i] = j;
        }
    }

    void _getNext(int* next, string needle) {
        int j = -1;
        next[0] = j;
        for (int i = 1; i < needle.size(); i++) {
            // “遇见冲突看next的前一位”为循环不变量，j根据已有的next数组进行回退
            while (j >= 0 && needle[i] != needle[j + 1]) {
                j = next[j];
            } 
            if (needle[i] == needle[j + 1]) {
                j++;
            }
            next[i] = j;
        }
    }

    
private:


};

int main(int argc, char *argv[])
{
    std::string haystack = "hello";
    std::string needle = "l";
    Solution solution;
    cout << "common index in haystack: " << solution.strStr(haystack, needle) << endl;
    /* code */

    return 0;
}
