#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// 判断两个字符串是否只差一个字符
bool isOneCharDiff(const string& s1, const string& s2) {
    if (s1.length() != s2.length()) return false;
    int diffCount = 0;
    for (size_t i = 0; i < s1.length(); i++) {
        if (s1[i] != s2[i]) {
            diffCount++;
            if (diffCount > 1) return false;
        }
    }
    return diffCount == 1;
}

// 构建邻接表图
unordered_map<string, vector<string>> buildGraph(const vector<string>& wordList, const string& beginStr, const string& endStr) {
    unordered_map<string, vector<string>> graph;
    vector<string> allWords = wordList;
    
    // 将起始字符串加入到单词列表中（如果不存在）
    bool hasBeginEndStr = false;
    for (const string& word : wordList) {
        if (word == beginStr || word == endStr) {
            hasBeginEndStr = true;
            break;
        }
    }
    if (!hasBeginEndStr) {
        allWords.push_back(beginStr);
        allWords.push_back(endStr);
    }
    
    // 构建邻接表：两两比较，只差一个字符的单词相连
    for (int i = 0; i < allWords.size(); i++) {
        for (int j = i + 1; j < allWords.size(); j++) {
            if (isOneCharDiff(allWords[i], allWords[j])) {
                graph[allWords[i]].push_back(allWords[j]);
                graph[allWords[j]].push_back(allWords[i]);
            }
        }
    }
    
    return graph;
}

// BFS搜索最短路径
int findShortestPath(const unordered_map<string, vector<string>>& graph, const string& beginStr, const string& endStr) {
    // 如果终点不在图中，返回0
    if (graph.find(endStr) == graph.end()) return 0;
    
    queue<string> q;
    unordered_map<string, int> distance;
    
    q.push(beginStr);
    distance[beginStr] = 1;
    
    while (!q.empty()) {
        string current = q.front();
        q.pop();
        
        // 找到目标字符串
        if (current == endStr) {
            return distance[current];
        }
        
        // 遍历当前字符串的所有邻居
        if (graph.find(current) != graph.end()) {
            for (const string& neighbor : graph.at(current)) {
                if (distance.find(neighbor) == distance.end()) { // 未访问过
                    distance[neighbor] = distance[current] + 1;
                    q.push(neighbor);
                }
            }
        }
    }
    
    return 0; // 找不到路径
}

int main() {
    int stringListSize;
    cin >> stringListSize;
    
    string beginStr, endStr;
    cin >> beginStr >> endStr;
    
    vector<string> stringList(stringListSize);
    for (int i = 0; i < stringListSize; i++) {
        cin >> stringList[i];
    }

    // 方法2：先构建图，再搜索最短路径
    unordered_map<string, vector<string>> graph = buildGraph(stringList, beginStr, endStr);
    int result = findShortestPath(graph, beginStr, endStr);
    cout << result << endl;
    
    return 0;
}
