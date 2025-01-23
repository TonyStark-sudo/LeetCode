#include <vector>

using namespace std;

void mySwap(char& a, char& b) {
    char temp;
    temp = a;
    a = b;
    b = temp;
}

class Solution {
public:


    void reverseString(vector<char>& s) {
        int left = 0, right = s.size() - 1;
        while (left < right) {
            mySwap(s[left], s[right]);
            left++;
            right--;
        }
    }
};
