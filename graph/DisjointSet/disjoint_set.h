#include <vector>
#include <iostream>

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

// int main() {
//     // 创建4个节点的并查集
//     DisjointSet ds(6);
//     cout << "初始状态:" << endl;
//     ds.printStatus();
    
//     // 模拟你原来想要的关系
//     ds.unite(0, 2);  // 连接节点0和2
//     ds.unite(1, 3);  // 连接节点1和3
//     ds.unite(2, 3);  // 连接节点2和3（间接连接所有节点）
    
//     cout << "\n合并后状态:" << endl;
//     ds.printStatus();
    
//     // 测试连通性
//     cout << "\n连通性测试:" << endl;
//     cout << "节点1和2是否连通: " << (ds.isConnected(1, 2) ? "是" : "否") << endl;
//     cout << "节点0和3是否连通: " << (ds.isConnected(0, 3) ? "是" : "否") << endl;
    
//     return 0;
// }