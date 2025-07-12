#include <iostream>
#include <vector>
#include <list>

using namespace std;

vector<vector<int>> result;
vector<int> path;

void dfs(const vector<list<int>>& graph, int cur, int target) {
    if (cur == target) {
        result.push_back(path);
        return ;
    }
    for (int x : graph[cur]) {
        path.push_back(x);
        dfs(graph, x, target);
        path.pop_back();
    }
}
int main(int argc, char const *argv[])
{
    int n, m;
    cin >> n >> m;
    vector<list<int>> graph(n + 1);
    while (m--) {
        int s, t;
        cin >> s >> t;
        graph[s].push_back(t);
    }
    path.emplace_back(1);
    dfs(graph, 1, n);
    if (result.size() == 0) cout << -1 << endl;
    for (auto& pa : result) {
        for (int i = 0; i < pa.size() - 1; i++) {
            cout << pa[i] << " ";
        }
        cout << pa[pa.size() - 1] << endl;
    }
    return 0;
}
