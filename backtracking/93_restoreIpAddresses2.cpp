#include <vector>
#include <string>

using namespace std;
class Solution {
private:
vector<string> results;
bool isSegment(const string& segment) {
    if (segment[0] == '0' && segment.size() > 1) return false;
    return stoi(segment) >= 0 && stoi(segment) <= 255;
}

void backtracking(const string& s, int startIndex, int segCount, string result) {
    if (segCount == 4 && startIndex >= s.size()) {
        result.pop_back();
        results.push_back(result);
        return ;
    } 
    if (segCount > 4) return ;
    for (int i = 1; i <= 3 && startIndex + i <= s.size(); i++) {
        string seg = s.substr(startIndex, i);
        if (isSegment(seg)) {
            backtracking(s, startIndex + i, segCount + 1, result + seg + ".");
        }
    }
}
public:
    vector<string> restoreIpAddresses(string s) {
        backtracking(s, 0, 0, "");
        return results;
    }
};