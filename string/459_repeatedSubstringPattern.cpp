#include <string>
#include <vector>
#include <iostream>

class Solution {
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

int main(int argc, char *argv[])
{
    std::string  s = "abcab";
    Solution solution;
    std::cout << "ans is: " << solution._repeatedSubstringPattern(s) << std::endl;
    /* code */
    return 0;
}
