#include <vector>
#include <iostream>

using namespace std;

int n, m;
int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};

void dfs(const vector<vector<int>>& grid, vector<vector<int>>& fromPosition, int x, int y) {
    fromPosition[x][y] = 1;
    for (int i = 0; i < 4; i++) {
        int next_x = x + dir[i][0];
        int next_y = y + dir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= grid.size() || next_y >= grid[0].size()) continue;
        if (grid[x][y] <= grid[next_x][next_y] && !fromPosition[next_x][next_y]) {
            dfs(grid, fromPosition, next_x, next_y);
        }
    }
}   

int main(int argc, char const *argv[])
{
    cin >> n >> m;
    vector<vector<int>> grid(n, vector<int>(m, 0));
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> grid[i][j];
        }
    }

    vector<vector<int>> tofirstBorder(n, vector<int>(m, 0));
    vector<vector<int>> tosecondBorder(n, vector<int>(m, 0));

    for (int i = 0; i < n; i++) {
        dfs(grid, tofirstBorder, i, 0);
        dfs(grid, tosecondBorder, i, m - 1);
    }
    for (int i = 0; i < m; i++) {
        dfs(grid, tofirstBorder, 0, i);
        dfs(grid, tosecondBorder, n - 1, i);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (tofirstBorder[i][j] && tosecondBorder[i][j]) {
                cout << i << " " << j << endl;
            }
        }
    }
    return 0;
}
