#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};

void dfs(vector<vector<int>>& graph, vector<vector<int>>& visited, int x, int y, bool& isAlone) {
    if (visited[x][y] || !graph[x][y]) {
        return ;
    }
    visited[x][y] = 1;
    for (int i = 0; i < 4; i++) {
        int next_x = x + dir[i][0];
        int next_y = y + dir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= graph.size() || next_y >= graph[0].size()) {
            continue;
        }
        if (!visited[next_x][next_y] && graph[next_x][next_y]) {
            dfs(graph, visited, next_x, next_y, isAlone);
            if (next_x == 0 || next_y == 0 || next_x == graph.size() - 1 || next_y == graph[0].size() - 1) {
                isAlone = false;
            }
        }
    }
}

void mergeAloneIsland(vector<vector<int>>& graph, vector<vector<int>>& merged, int x, int y) {
    queue<pair<int, int>> island_part;
    island_part.push({x, y});
    graph[x][y] = 0;
    merged[x][y] = 1;
    while (!island_part.empty()) {
        pair<int, int> part_xy = island_part.front();
        island_part.pop();
        for (int i = 0; i < 4; i++) {
            int next_x = part_xy.first + dir[i][0];
            int next_y = part_xy.second + dir[i][1];
            if (next_x < 0 || next_y < 0 || next_x >= graph.size() || next_y >= graph[0].size()) continue;
            if (!merged[next_x][next_y] && graph[next_x][next_y]) {
                island_part.push({next_x, next_y});
                graph[next_x][next_y] = 0;
                merged[next_x][next_y] = 1;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    int n, m;
    cin >> n >> m;
    vector<vector<int>> graph(n, vector<int>(m, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cin >> graph[i][j];
        }
    }
    vector<vector<int>> visited(n, vector<int>(m, 0));
    vector<vector<int>> merged(n, vector<int>(m, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (graph[i][j] && !visited[i][j]) {
                bool isAlone = true;
                if (i == 0 || i == (n - 1) || j == 0 || j == (m - 1)) {
                    isAlone = false;
                }
                dfs(graph, visited, i, j, isAlone);
                cout << "This island (" << i << ", " << j << ") is ALone? " << isAlone << endl;
                if (isAlone) {
                    mergeAloneIsland(graph, merged, i, j);
                }
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cout << graph[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}
