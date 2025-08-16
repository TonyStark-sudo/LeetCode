#include <iostream>
#include <vector>

using namespace std;

int N;

class UnionFind {
private:
    vector<int> father_;
public:
    UnionFind(int nodeNum) : father_(nodeNum) {
        for (int i = 0; i < nodeNum; i++) {
            father_[i] = i;
        }
    }
    int find(int a) {
        if (father_[a] == a) return father_[a];
        father_[a] = find(father_[a]);
        return father_[a];
    }
    void Union(int a, int b) {
        int rootA = find(a);
        int rootB = find(b);
        if (rootA != rootB) father_[rootA] = rootB;
        return ;
    }
    bool isSame(int a, int b) {
        int rootA = find(a);
        int rootB = find(b);
        return rootA == rootB;
    }
};

bool isTreeAfterRemoveEdge(const vector<vector<int>>& edges, int deleteEdge) {
    UnionFind unionfind(N);
    for (int i = 0; i < N; i++) {
        if (i == deleteEdge) continue;
        if (unionfind.isSame(edges[i][0], edges[i][1])) {
            return false;
        }
        unionfind.Union(edges[i][0], edges[i][1]);
    }
    return true;
}

void getRemoveEdge(const vector<vector<int>>& edges) {
    UnionFind unionfind(N);
    for (int i = 0; i < N; i++) {
        if (unionfind.isSame(edges[i][0], edges[i][1])) {
            cout << edges[i][0] << " " << edges[i][1] << endl;
            return ;
        }
        else {
            unionfind.Union(edges[i][0], edges[i][1]);
        }
    }
}

int main(int argc, char const *argv[])
{
    int s, t;
    vector<vector<int>> edges;
    cin >> N;
    vector<int> inDegree(N + 1, 0);
    for (int i = 0; i < N; i++) {
        cin >> s >> t;
        inDegree[t]++;
        edges.push_back({s, t});
    }
    vector<int> vec;
    for (int i = N - 1; i >= 0; i--) {
        if (inDegree[edges[i][1]] == 2) {
            vec.push_back(i);
        }
    }
    if (vec.size() > 0) {
        if (isTreeAfterRemoveEdge(edges, vec[0])) {
            cout << edges[vec[0]][0] << " " << edges[vec[0]][1] << endl;
        }
        else {
            cout << edges[vec[1]][0] << " " << edges[vec[1]][1] << endl;
        }
        return 0;
    }
    getRemoveEdge(edges);
    return 0;
}
