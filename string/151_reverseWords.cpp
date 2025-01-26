#include <iostream>
#include <string>

using namespace std;

void removeExtraSpace(string& s) {
    for (int i = s.size() - 1; i > 0; i--) {
        if (s[i - 1] == s[i] && s[i] == ' ') s.erase(s.begin() + i);
        if (s.size() > 0 && s[s.size() - 1] == ' ') s.erase(s.begin() + s.size() - 1);
        if (s.size() > 0 && s[0] == ' ') s.erase(s.begin());
    }
}
class Solution {
public:
    void removeExtraSpacesV1(string& s) {
        for (int i = s.size() - 1; i > 0; i--) {
            if (s[i - 1] == s[i] && s[i] == ' ') s.erase(s.begin() + i);
            if (s.size() > 0 && s[s.size() - 1] == ' ') s.erase(s.begin() + s.size() - 1);
            if (s.size() > 0 && s[0] == ' ') s.erase(s.begin());
        }
    }

    void removeExtraSpaceV2(string& s) {
        int slow = 0, fast = 0;
        while (s.size() > 0 && fast < s.size() && s[fast] == ' ') {
            fast++;
        }

        for (; fast < s.size(); fast++) {
            if (fast - 1 > 0
                && s[fast - 1] == s[fast]
                && s[fast] == ' ') continue;
            else s[slow++] = s[fast];

        }

        if (slow - 1 > 0 && s[slow - 1] == ' ') s.resize(slow - 1);
        else s.resize(slow);

    }

    void reverse(string& s, int start, int end) {
        for (int i = start, j = end; i < j; i++, j--) {
            swap(s[i], s[j]);
        }
    }

    string reverseWords(string s) {
        removeExtraSpace(s);
        reverse(s, 0, s.size() - 1);
        int start = 0, end = 0;
        for (int i = end; i < s.size(); i++) {
            if (s[i] == ' ') {
                reverse(s, start, i - 1);
                start = i + 1;
            }

            if (i == s.size() - 1) reverse(s, start, i);
        }
        return s;
    }
};

int main(int argc, char *argv[])
{
    /* code */
    string s = " 12222 21111 3  45";
    Solution solution;
    cout << "the anw: \n" << solution.reverseWords(s) << endl;
    for (auto _char : s) {
        cout << _char << " ";
    }
    cout << "size of original: " << s.size() << "\n";
    // removeExtraSpace(s);
    solution.removeExtraSpaceV2(s);
    for (auto _char : s) {
        cout << _char << " ";
    }
    cout << "size of changed: " << s.size() << "\n";
    return 0;
}
