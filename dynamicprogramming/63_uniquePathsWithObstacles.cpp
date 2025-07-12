#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        int row = obstacleGrid.size(), col = obstacleGrid[0].size();
        vector<vector<int>> dp(row, vector<int>(col, 0));
        for (int i = 0; i < row; i++) {
            if (obstacleGrid[i][0] == 1) {
                break;
            }
            dp[i][0] = 1;
        }
        for (int j = 0; j < col; j++) {
            if (obstacleGrid[0][j] == 1) {
                break;
            }
            dp[0][j] = 1;
        }
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                std::cout << dp[i][j] << " ";
            }
            std::cout << std::endl;
        }
        for (int i = 1; i < row; i++) {
            for (int j = 1; j < col; j++) {
                if (obstacleGrid[i][j] == 1) {
                    dp[i][j] = 0;
                }
                else 
                {
                    dp[i][j] = dp[i][j - 1] + dp[i - 1][j];
                }
            }
        }
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                std::cout << dp[i][j] << " ";
            }
            std::cout << std::endl;
        }
        return dp[row - 1][col - 1];
    }
};

int main(int argc, char const *argv[])
{
    vector<vector<int>> map {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
    Solution solution;
    int path_num = solution.uniquePathsWithObstacles(map);
    return 0;
}
