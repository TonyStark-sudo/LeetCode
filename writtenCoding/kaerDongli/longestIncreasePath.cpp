#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

vector<vector<int>> dir{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
// int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

int dfs(const vector<vector<int>>& matrix, int x, int y, vector<vector<int>>& memo) {
    if (memo[x][y] != 0) return memo[x][y];
    int maxlen = 1;
    for (auto& d : dir) {
        int nx = x + d[0], ny = y + d[1];
        if (nx >= 0 && nx < matrix.size() && ny >= 0 && ny < matrix[0].size() && matrix[nx][ny] > matrix[x][y]) {
            maxlen = max(maxlen, 1 + dfs(matrix, nx, ny, memo));
        }
    }
    memo[x][y] = maxlen;
    return maxlen;
}

int longestIncreasingPath(vector<vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) return 0;
    int m = matrix.size(), n = matrix[0].size(), res = 0;
    vector<vector<int>> memo(m, vector<int>(n, 0));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            res = max(res, dfs(matrix, i, j, memo));
        }
    }
    return res;
}

int main() {
    vector<vector<int>> matrix = {
        {9,9,4},
        {6,6,8},
        {2,1,1}
    };
    cout << longestIncreasingPath(matrix) << endl; // 输出4
    return 0;
}