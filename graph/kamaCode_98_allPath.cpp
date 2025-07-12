#include <iostream>
#include <vector>

using namespace std;

vector<vector<int>> result;
vector<int> path;

void dfs(const vector<vector<int>>& graph, int cur, int n) {
  if (cur == n) {
    result.push_back(path);
    return ;
  }
  for (int i = 1; i <= n; i++) {
    if (graph[cur][i]) {
      path.push_back(i);
      dfs(graph, i, n);
      path.pop_back();
    }
  }
}

int main() {
  int m, n;
  std::cin >> m >> n;
  vector<vector<int>> graph(n + 1, vector<int>(n + 1, 0));
  while (m--) {
    int s, t;
    std::cin >> s >> t;
    graph[s][t] = 1;
  }
  path.emplace_back(1);
  dfs(graph, 1, n);
  if (result.size() == 0) cout << -1 << endl;;
//   for (int i = 0; i < result.size(); i++) {
  for (auto& res : result) {
    for (int i = 0; i < res.size() - 1; ++i) {
        cout << res[i] << " ";
    }
    cout << res[res.size() - 1] << endl;
  }
  return 0;

}