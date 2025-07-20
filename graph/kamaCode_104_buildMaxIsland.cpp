#include <iostream>
#include <vector>

using namespace std;

int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};
int m, n;

bool isIslandNeighbor(const vector<vector<int>>& grid, int x, int y) {
    for (int i = 0; i < 4; i++) {
        int next_x = x + dir[i][0];
        int next_y = y + dir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= grid.size() || next_y >= grid[0].size()) continue;
        if (grid[next_x][next_y]) return true;
    }
    return false;
}

void dfs(const vector<vector<int>>& grid, vector<vector<int>>& visited, int x, int y, int& sum) {
    visited[x][y] = 1;
    sum++;
    for (int i = 0; i < 4; i++) {
        int next_x = x + dir[i][0];
        int next_y = y + dir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= grid.size() || next_y >= grid[0].size()) continue;
        if (grid[next_x][next_y] && !visited[next_x][next_y]) {
            dfs(grid, visited, next_x, next_y, sum);
        }
    }
}

int main(int argc, char const *argv[])
{
    cin >> n >> m;
    vector<vector<int>> grid(n, vector<int>(m, 0));
    for (size_t i{0UL}; i < n; ++i) {
        for (size_t j{0UL}; j < m; ++j) {
            cin >> grid[i][j];
        }
    }
    int maxSquare = 0;
    bool merge = false;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            vector<vector<int>> visited(n, vector<int>(m, 0));
            if (!grid[i][j] && isIslandNeighbor(grid, i, j)) {
                merge = true;
                // cout << "i = " << i << " j = " << j << endl;
                int sum = 0;
                dfs(grid, visited, i, j, sum);
                maxSquare = max(sum, maxSquare);
                // cout << "maxSquare: " << maxSquare <<endl;
            }
        }
    }
    if (merge) {
        cout << maxSquare << endl;
    }
    else if (!grid[0][0]) {
        cout << 1 << endl;
    }
    else 
    {
        cout << m * n << endl;
    }
    return 0;
}
