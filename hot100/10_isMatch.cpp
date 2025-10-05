#include <string>
#include <map>
#include <iostream>

using namespace std;

class Solution {
public:
    bool isMatch(string s, string p) {
        // multimap<char, char> map{
        //     {'.', 'a'},
        //     {'.', 'b'}
        // };
        // return true;
        int j = 0;        
        for (int i = 0; i < p.size(); i++) {
            while (p[i] == '*') {
                if (s[j] == s[j + 1]) {
                    j++;
                }
                else break;       
            } 
            if (p[i] == '.') {

            }
            if (p[i] != '.' && p[i] != '*')
            j++;
        }
    }
};

int main(int argc, char *argv[])
{
    multimap<char, char> map{
        {'.', 'a'},
        {'.', 'b'}
    };
    auto iter = map.find('.');
    int count = map.count('.');
    cout << "count: " << count << endl;
    cout << "'.' - " << iter->second << endl;
    /* code */
    return 0;
}
