// 本体思路：通过visited数组判断是否是访问过的陆地，然后使用dfs搜索该陆地相邻的陆地并标记访问过的陆地，在外循环中
// 每一次进入深搜说明找到了一块岛屿
#include <vector>
#include <iostream>

using namespace std;

int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};
void dfs(const vector<vector<int>>& graph, int x, int y, vector<vector<int>>& visited) {
    for (int i = 0; i < 4; i++) {
        int next_x = x + dir[i][0];
        int next_y = y + dir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= graph.size() || next_y >= graph[0].size()) continue;
        if (graph[next_x][next_y] && !visited[next_x][next_y]) {
            visited[next_x][next_y] = 1;
            dfs(graph, next_x, next_y, visited);
        }
    }
}

void dfs2(const vector<vector<int>>& graph, int x, int y, vector<vector<int>>& visited) {
    if (visited[x][y] || !graph[x][y]) {
        return ;
    }
    visited[x][y] = 1;
    for (int i = 0; i < 4; ++i) {
        int next_x = x + dir[i][0];
        int next_y = y + dir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= graph.size() || next_y >= graph[0].size()) continue;
        dfs2(graph, next_x, next_y, visited);
    }
}

int main(int argc, char const *argv[])
{
    int n, m;
    cin >> n >> m;
    vector<vector<int>> graph(n, vector<int>(m, 0));
    for (size_t i{0UL}; i < n; i++) {
        for (size_t j{0UL}; j < m; j++) {
            cin >> graph[i][j];
        }
    }
    vector<vector<int>> visited(n, vector<int>(m, 0));
    int islandNum = 0;
    for (size_t i{0UL}; i < n; i++) {
        for (size_t j{0UL}; j < m; j++) {
            if (graph[i][j] && !visited[i][j]) {
                islandNum++;
                // visited[i][j] = 1;
                dfs2(graph, i, j, visited);
            }
        }
    }
    std::cout << islandNum << std::endl;
    return 0;
}
