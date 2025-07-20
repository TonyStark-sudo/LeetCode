#include <iostream>
#include <vector>

using namespace std;

int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};
int rightdownDir[2][2] = {0, 1, 1, 0};
int leftupDir[2][2] = {0, -1, -1, 0};

void toLeftUp(const vector<vector<int>>& graph, bool& arriveLeftUp, vector<vector<int>> visited, int x, int y) {
    visited[x][y] = 1;
    if (x == 0 || y == 0) {
        arriveLeftUp = true;
        return ;
    } 
    for (int i = 0; i < 2; i++) {
        int next_x = x + leftupDir[i][0];
        int next_y = y + leftupDir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= graph.size() || next_y >= graph[0].size()) continue;
        if (graph[next_x][next_y] <= graph[x][y] && !visited[next_x][next_y]) toLeftUp(graph, arriveLeftUp, visited, next_x, next_y);
    }
}

void toRightDown(const vector<vector<int>>& graph, bool& arriveRightDown, vector<vector<int>> visited, int x, int y) {
    visited[x][y] = 1;
    if (x == graph.size() - 1 || y == graph[0].size() - 1) {
        arriveRightDown = true;
        return ;
    }
    for (int i = 0; i < 2; i++) {
        int next_x = x + rightdownDir[i][0];
        int next_y = y + rightdownDir[i][1];
        if (next_x < 0 || next_y < 0 || next_x >= graph.size() || next_y >= graph[0].size()) continue;
        if (graph[next_x][next_y] <= graph[x][y] && !visited[next_x][next_y]) toRightDown(graph, arriveRightDown, visited, next_x, next_y);
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
    vector<vector<int>> visited_1(n, vector<int>(m, 0));
    vector<vector<int>> visited_2(n, vector<int>(m, 0));
    for (size_t i{0UL}; i < n; ++i) {
        for (size_t j{0UL}; j < m; ++j) {
            bool arrivedLU = false, arrivedRD = false;
            toLeftUp(graph, arrivedLU, visited_1, i, j);
            toRightDown(graph, arrivedRD, visited_2, i, j);
            if (arrivedLU && arrivedRD) {
                cout << i << " " << j << endl;
            }
        }
    }
    return 0;
}
