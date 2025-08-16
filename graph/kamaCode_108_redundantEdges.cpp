#include <iostream>
#include <vector>

using namespace std;

class UnionFind {
private:
    vector<int> father_;
public:
    UnionFind(int n) : father_(n) {
        for (int i = 0; i < father_.size(); ++i) {
            father_[i] = i;
        }
    }
    int find(int node) {
        if (father_[node] == node) return father_[node];
        father_[node] = find(father_[node]);
        return father_[node];
    }
    void Union(int a, int b) {
        int rootA = find(a);
        int rootB = find(b);
        if (rootA == rootB) return ;
        father_[rootA] = rootB;
    }
    bool isSame(int a, int b) {
        int rootA = find(a);
        int rootB = find(b);
        if (rootA == rootB) return true;
        return false; 
    }
};

int main(int argc, char const *argv[])
{
    int nodeNum, edgeNum;
    cin >> nodeNum;
    edgeNum = nodeNum;
    UnionFind unionfind(nodeNum);
    while ((nodeNum--) != 0) {
        int s, t;
        cin >> s >> t;
	    if (unionfind.isSame(s, t)) {
            cout << s << " " << t << endl;
            return 0;
	    }
        else {
            unionfind.Union(s, t);
        }
    }
    return 0;
}
