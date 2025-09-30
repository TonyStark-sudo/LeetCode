#include <vector>
#include <queue>

using namespace std;

class Solution {
private:
    int dir[4][2] = {
        {0, 1},
        {1, 0},
        {0, -1}, 
        {-1, 0}
    };
    queue<pair<int, int>> q;
    vector<vector<bool>> visited;
    int island_num;
    void init(const vector<vector<char>>& grid) {
        visited = vector<vector<bool>>(grid.size(), vector<bool>(grid.at(0).size(), false));
        island_num = 0;
    }
    void bfs(const vector<vector<char>>& grid, int x, int y, vector<vector<bool>>& visited) {
        // if (!visited[x][y] && grid[x][y] == '1') {
        //     q.push({x, y});
        //     visited[x][y] = true;
        // }
        q.push({x, y});
        visited[x][y] = true;
        while (!q.empty()) {
            pair<int, int> cur = q.front();
            q.pop();
            for (int i = 0; i < 4; i++) {
                int nx = cur.first + dir[i][0];
                int ny = cur.second + dir[i][1];
                if (nx < 0 || nx >= grid.size() || ny < 0 || ny >= grid[0].size()) continue;
                if (!visited[nx][ny] && grid[nx][ny] == '1') {
                    q.push({nx, ny});
                    visited[nx][ny] = true;
                }
            }
        }
    }
    void dfs(const vector<vector<char>> grid, int x, int y, vector<vector<bool>> visited) {
        if (visited[x][y] || grid[x][y] == '0') {
            return ;
        }
        visited[x][y] = 1;
        for (int i = 0; i < 4; i++) {
            int nx = x + dir[i][0];
            int ny = y + dir[i][1];
            if (nx < 0 || ny < 0 || nx >= grid.size() || ny >= grid[0].size()) continue;
            dfs(grid, nx, ny, visited);
        }
    }

public:
    int numIslands(vector<vector<char>>& grid) {
        init(grid);
        for (size_t i{0UL}; i < grid.size(); i++) {
            for (size_t j{0UL}; j < grid[0].size(); j++) {
                if (grid[i][j] == '1' && !visited[i][j]) {
                    island_num++;
                    bfs(grid, i, j, visited);
                }
            }
        }
        return island_num;
    }
};