#include <vector>

using namespace std;

class Solution {
public:
    int uniquePaths(int m, int n) {
        vector<vector<int>> paths(m, vector<int>(n, 0));
        for (int i = 0; i < m; i++) {
            paths[i][0] = 1;
        }
        for (size_t i{0UL}; i < n; i++) {
            paths[0][i] = 1;
        }
        for (int i = 1; i < m; i++) {
            for (int j = 1; j < n; j++) {
                paths[i][j] = paths[i][j - 1] + paths[i - 1][j];
            }
        }
        return paths[m - 1][n - 1];        
    }
};