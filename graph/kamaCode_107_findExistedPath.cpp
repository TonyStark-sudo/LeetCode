#include <iostream>
#include <vector>
// #include "disjoint_set.h"

using namespace std;

class DisjointSet {
private:
    vector<int> parent;
    
public:
    // 构造函数：初始化n个节点
    DisjointSet(int n) : parent(n) {
        // 每个节点初始时指向自己
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }
    
    // 查找根节点（带路径压缩）
    int find(int node) {
        if (parent[node] != node) {
            parent[node] = find(parent[node]);  // 路径压缩
        }
        return parent[node];
    }
    
    // 合并两个集合
    void unite(int a, int b) {
        int rootA = find(a);
        int rootB = find(b);
        
        if (rootA != rootB) {
            parent[rootA] = rootB;  // 让根节点相连
        }
    }
    
    // 判断是否在同一集合
    bool isConnected(int a, int b) {
        return find(a) == find(b);
    }
    
    // 打印当前状态
    void printStatus() {
        cout << "Parent array: ";
        for (int i = 0; i < parent.size(); i++) {
            cout << i << "→" << parent[i] << " ";
        }
        cout << endl;
    }
};

int main(int argc, char const *argv[])
{
    int nodeNum, sideNum;
    cin >> nodeNum >> sideNum;
    DisjointSet dis(nodeNum + 1);
    while (static_cast<bool>(sideNum--)) {
        int row, col;
        cin >> row >> col;
        dis.unite(row, col);
    }
    int source, desti;
    cin >> source >> desti;
    cout << dis.isConnected(source, desti) << endl;
    return 0;
}