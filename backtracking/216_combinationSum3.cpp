#include <vector>
using namespace std;

class Solution {
private:
    vector<int> path;
    vector<vector<int>> result;
    int sum = 0;
    void backtracking(int k, int target, int startIndex) {
        if (path.size() > k || sum > target) {
            return ;
        }
        if (path.size() == k && sum == target) {
            result.push_back(path);
            return ;
        }
        for (int i = startIndex; i < 10; i++) {
            sum += i;
            path.push_back(i);
            backtracking(k, target, i + 1);
            path.pop_back();
            sum -= i; 
        }
    }
public:
    vector<vector<int>> combinationSum3(int k, int n) {
        backtracking(k, n, 0);
        return result;
    }
};