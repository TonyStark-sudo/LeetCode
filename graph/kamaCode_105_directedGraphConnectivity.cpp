#include <list>
#include <iostream>
#include <vector>

using namespace std;

int nodeNum, sideNum;

void dfs(const vector<list<int>>& graph, int node, vector<bool>& visited) {
    if (visited[node] == true) {
        return ;
    }
    visited[node] = true;
    for (int neighbor : graph[node]) {
        dfs(graph, neighbor, visited);
    }
}

int main(int argc, char const *argv[])
{
    cin >> nodeNum >> sideNum;
    // vector<vector<int>> graph(nodeNum, vector<int>(nodeNum, 0));
    // for (int i = 0; i < sideNum; i++) {
    //     int row, col;
    //     cin >> row >> col;
    //     graph[row][col] = 1;
    // }
    vector<list<int>> graph(nodeNum + 1);
    vector<bool> visited(nodeNum + 1, false);
    int s, t;
    for (int i = 0; i < sideNum; i++) {
        cin >> s >> t;
        graph[s].push_back(t);
    }

    dfs(graph, 1, visited);
    for (int i = 1; i <= nodeNum; i++) {
        if (!visited[i]) {
            cout << -1 << endl;
            return 0;
        }
    }
    cout << 1 << endl;
    return 0;
}
