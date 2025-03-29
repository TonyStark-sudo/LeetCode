#include <vector>

using namespace std;

class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        // vector<vector<int>> dp(m, vector<int>(n, 0));
        int m = obstacleGrid.size();
        int n = obstacleGrid[0].size();
        
        vector<vector<int>> dp(m, vector<int>(n, 0));
        for (int i = 0; i < obstacleGrid.size(); i++) {
            if (obstacleGrid[i][0] != 1) dp[i][0] = 1;
            else break;
        }
        for (int i = 0; i < obstacleGrid[0].size(); i++) {
            if (obstacleGrid[0][i] != 1) dp[0][i] = 1;
            else break;
        }
        // if (m <= 1 || n <= 1) return 0;
        for (int i = 1; i < obstacleGrid.size(); i++) {
            for (int j = 1; j < obstacleGrid[i].size(); j++) {
                if (obstacleGrid[i][j] != 1) {
                    dp[i][j] = dp[i][j - 1] + dp[i - 1][j];
                }
                else dp[i][j] = 0;
            }
        }
        return dp[m - 1][n - 1];
    }
};