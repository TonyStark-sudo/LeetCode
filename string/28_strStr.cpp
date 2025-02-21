#include <iostream>
#include <string>

using namespace std;
class Solution {
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
public:
    int strStr(string haystack, string needle) {
        // int next[needle.size()];
        // int* next = new int[needle.size()]; // 动态分配内存，需要手动释放 delete[] next;
        int* next = (int*)malloc(needle.size() * sizeof(int)); // 动态分配内存，需要手动释放 free(next);
        getNext(next, needle);
        int j = 0;
        for (size_t i = 0; i < haystack.size(); i++) {
            while (j > 0 &&  haystack[i] != needle[j]) {
                j = next[j - 1];
            }
            if (haystack[i] == needle[j]) {
                j++;
            }
            if (j == needle.size()) return i - needle.size() + 1;
        }
        free(next);
        return -1;
    }

    void getNext(int* next, string s) {
        int j = 0;
        next[0] = j;
        for (size_t i = 1; i < s.size(); i++) {
            while (j >0 && s[i] != s[j]) {
                j = next[j - 1];
            }
            if (s[i] == s[j]) {
                j++;
            }
            next[i] = j;
        }
    }
};

int main(int argc, char *argv[])
{
    // string haystack = "leetcode", needle = "leeto";
    string haystack = "abcabcabcabcabc";
    int next[haystack.size()];
    Solution solution;
    solution.getNext(next, haystack);
    cout << "next array:\n";
    for (int _ : next) {
        cout << _ << " ";
    }
    // cout << "prefix index: " << solution.strStr(haystack, needle) << endl;
    /* code */
    return 0;
}
