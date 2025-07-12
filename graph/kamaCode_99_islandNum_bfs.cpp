#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};
// vector<vector<int>> dir {
//     {0, 1},
//     {1, 0},
//     {0, -1},
//     {-1, 0}
// };

int island_num;
queue<pair<int, int>> elem_queue;
void bfs(const vector<vector<int>>& graph, vector<vector<int>>& visited, int x, int y) {
    // pair<int, int> cur(x, y);
    if (!visited[x][y] && graph[x][y]) {
        elem_queue.push({x, y});
        visited[x][y] = 1;
    }
    
    while (!elem_queue.empty())
    {
        pair<int, int> cur = elem_queue.front();
        elem_queue.pop();
        for (int i = 0; i < 4; i++) {
            // int next_x = x + dir[i][0];
            // int next_y = y + dir[i][1];
            int next_x = cur.first + dir[i][0];
            int next_y = cur.second + dir[i][1];
            if (next_x < 0 || next_y < 0 || next_x > graph.size() - 1 || next_y > graph[0].size() - 1) {
                continue;
            }
            if (!visited[next_x][next_y] && graph[next_x][next_y]) {
                elem_queue.push({next_x, next_y});
                visited[next_x][next_y] = 1;
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    int n, m;
    cin >> n >> m;
    vector<vector<int>> graph(n, vector<int>(m, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> graph[i][j];
        }
    }
    int island_num = 0;
    vector<vector<int>> visited(n, vector<int>(m, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (graph[i][j] == 1 && visited[i][j] != 1) {
                island_num++;
                bfs(graph, visited, i, j);
            }
        }
    }
    std::cout << island_num << std::endl;
    return 0;
}
