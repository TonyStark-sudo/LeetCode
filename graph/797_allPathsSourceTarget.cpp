#include <vector>

using namespace std;

class Solution {
private:
    vector<vector<int>> result;
    vector<int> path;
    void dfs(const vector<vector<int>>& graph, int cur, int node_num) {
        if (cur == node_num) {
            result.push_back(path);
            return ;
        }
        for (int i = 0; i < graph[cur].size(); ++i) {
            path.push_back(graph[cur][i]);
            dfs(graph, graph[cur][i], node_num);
            path.pop_back();
        }
    }
public:
    vector<vector<int>> allPathsSourceTarget(vector<vector<int>>& graph) {
        int node_num = graph.size();
        path.emplace_back(0);
        dfs(graph, 0, node_num - 1);
        return result;
    }
};