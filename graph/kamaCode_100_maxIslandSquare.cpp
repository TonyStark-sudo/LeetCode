#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};
queue<pair<int, int>> block_que;

int bfs(const vector<vector<int>>& graph, int x, int y, vector<vector<int>>& visited) {
    int land_num = 0;
    visited[x][y] = 1;
    block_que.emplace(x, y);
    land_num++;
    while(!block_que.empty()) {
        pair<int, int> cur = block_que.front();
        block_que.pop();
        for (int i = 0; i < 4; i++) {
            int next_x = cur.first + dir[i][0];
            int next_y = cur.second + dir[i][1];
            if (next_x < 0 || next_y < 0 || next_x >= graph.size() || next_y >= graph[0].size()) continue;
            if (graph[next_x][next_y] && !visited[next_x][next_y]) {
                block_que.emplace(next_x, next_y);
                visited[next_x][next_y] = 1;
                land_num++;
            }
        }
    }
    return land_num;
}

void dfs(const vector<vector<int>>& graph, int x, int y, vector<vector<int>>& visited, int& square) {
    for (int i = 0; i < 4; i++) {
        int next_x = x + dir[i][0];
        int next_y = y + dir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= graph.size() || next_y >= graph[0].size()) continue;
        if (graph[next_x][next_y] && !visited[next_x][next_y]) {
            visited[next_x][next_y] = 1;
            square++;
            dfs(graph, next_x, next_y, visited, square);
        }
    }
}
int main(int argc, char const *argv[])
{
    int n, m;
    cin >> n >> m;
    vector<vector<int>> graph(n, vector<int>(m));
    for (size_t i{0UL}; i < n; ++i) {
        for (size_t j{0UL}; j < m; ++j) {
            cin >> graph[i][j];
        }
    }
    int maxSquare = 0;
    vector<vector<int>> visited(n, vector<int>(m, 0));
    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j < graph[0].size(); j++) {
            if (graph[i][j] && !visited[i][j]) {
                // int curSquare = bfs(graph, i, j, visited);
                int curSquare = 1;
                visited[i][j] = 1;
                dfs(graph, i, j, visited, curSquare);
                maxSquare = max(curSquare, maxSquare);
            }
        }
    }
    cout << maxSquare << endl;
    return 0;
}
