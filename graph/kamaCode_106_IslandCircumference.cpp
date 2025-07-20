#include <iostream>
#include <vector>

using namespace std;

int n, m;
int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};
int circumference = 0;

int calCirum(const vector<vector<int>>& grid, int x, int y) {
    bool hasWater = false;
    int circu = 0;
    for (int i = 0; i < 4; i++) {
        int next_x = x + dir[i][0];
        int next_y = y + dir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= grid.size() || next_y >= grid[0].size()) {
            circu++;
            // hasWater = true;
            continue;
        }
        if (!grid[next_x][next_y]) {
            circu++;
            // hasWater = true;
        }
    }
    // cout << "(" << x << ", " << y << ") circu is " << circu << endl; 
    return circu;
}

// int cir = 0;
// void dfs(const vector<vector<int>>& grid, vector<vector<int>>& visited, int x, int y) {
//     if (visited[x][y] || !grid[x][y]) {
//         return ;
//     }
//     visited[x][y] = 1;
//     cir += calCirum(grid, x, y);
//     for (int i = 0; i < 4; i++) {
//         int next_x = x + dir[i][0];
//         int next_y = x + dir[i][1];
//         if (next_x < 0 || next_y < 0 || next_x >= grid.size() || next_y >= grid[0].size() - 1) continue;
//         if (!visited[next_x][next_y] && grid[next_x][next_y]) {
//             dfs(grid, visited, next_x, next_y);
//         }
//     }
// }

int main(int argc, char const *argv[])
{
    cin >> n >> m;
    vector<vector<int>> grid(n, vector<int>(m, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> grid[i][j];
        }
    }

    // vector<vector<int>> visited(n, vector<int>(m, 0));
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < m; j++) {
    //         if (grid[i][j] && !visited[i][j]) {
    //             dfs(grid, visited, i, j);
    //         }
    //     } 
    // }
    vector<vector<int>> visited(n, vector<int>(m, 0));
    int circu = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (grid[i][j]) {
                circu += calCirum(grid, i, j);
            }
        } 
    }
    cout << circu <<endl;    
    return 0;
}
