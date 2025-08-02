#include <vector>
#include <string>

using namespace std;
class Solution {
private:
    int oneSum = 0;
    int zeroSum = 0;
    int m_, n_;
    int countOne(const string& str) {
        int count = 0;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == '1') count++;
        }
        return count;
    }
    // vector<vector<string>> results;
    vector<string> result;
    int maxSetNum = 0;
    void backtracking(const vector<string>& strs, int startIndex) {
        // 每次递归都更新最大值（当前状态合法）
        if (zeroSum <= m_ && oneSum <= n_) {
            maxSetNum = max(maxSetNum, (int)result.size());
        }
        // 如果超出限制或到达末尾，直接返回
        if (zeroSum > m_ || oneSum > n_ || startIndex >= strs.size()) {
            return;
        }
        
        for (int i = startIndex; i < strs.size(); ++i) {
            result.push_back(strs[i]);
            int oneNum = countOne(strs[i]);
            int zeroNum = strs[i].length() - oneNum;
            oneSum += oneNum;
            zeroSum += zeroNum;
            backtracking(strs, i + 1);
            oneSum -= oneNum; 
            zeroSum -= zeroNum;
            result.pop_back();
        }
    }

public:
    int findMaxForm(vector<string>& strs, int m, int n) {
        m_ = m;
        n_ = n;
        backtracking(strs, 0);
        return maxSetNum;
    }
};