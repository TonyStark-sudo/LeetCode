#include <queue>
#include <vector>
#include <iostream>

using namespace std;

int m = 4, n = 4;
int start_value = 1;
vector<vector<int>> graph(m, vector<int>(n));
vector<vector<int>> visited(m, vector<int>(n));
vector<vector<int>> direction {
    {0, 1}, // 右
    {1, 0}, // 下
    {0, -1}, // 左
    {-1, 0}, // 上
};

void bfs(const vector<vector<int>>& graph, vector<int>& path, int x, int y) {
    queue<pair<int, int>> element_que;
    element_que.push({x, y});
    visited[x][y] = 1;
    while(!element_que.empty()) {
        pair<int, int> cur = element_que.front();
        path.push_back(graph[cur.first][cur.second]);
        element_que.pop();
        for (int i = 0; i < direction.size(); ++i) {
            int next_x = cur.first + direction[i][0];
            int next_y = cur.second + direction[i][1];
            if (next_x < 0 || next_y < 0 || next_x > (m - 1) || next_y > (n - 1)) {
                continue;
            }
            if (visited[next_x][next_y] != 1) {
                element_que.emplace(next_x, next_y);
                visited[next_x][next_y] = 1;
            }

        }
    }    
}

int main(int argc, char const *argv[])
{
    // for (auto& line_vec : graph) {
    //     for (int& elem : line_vec) {
    //         cout << elem << " ";
    //     }
    //     cout << endl;
    // }
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            graph[i][j] = start_value++;
        }
    }
    for (auto& line_vec : graph) {
        for (int& elem : line_vec) {
            cout << elem << " ";
        }
        cout << endl;
    }
    vector<int> path;
    bfs(graph, path, 2, 1);
    for (int& _ : path) {
        cout << _ << " ";
    }
    cout << endl;
    return 0;
}
