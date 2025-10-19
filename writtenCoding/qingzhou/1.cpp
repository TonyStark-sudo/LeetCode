#include <iostream>
#include <vector>
#include <string>
using namespace std;

// 字母迷宫
// 字母迷宫游戏初始界面记作m * n二维字符串数组grid，请判断玩家是否能在grid中找到目标单词target
// 注意：寻找单词时必须按照字母顺序，通过水平或垂直方向相邻的单元格内的字母构成，同时不能重复使用单元格内的字母

//提示：1 <=m, n <= 6
//     1 <= target.length <= 15
//    grid和target仅由大小写英文字母组成
// 实例1：[["A","B","C","E"],["S","F","C","S"],["A","D","E","E"]], "ABCCED"
// 输出：true
// 实例2：[["A","B","C","E"],["S","F","C","S"],["A","D","E","E"]], "SEE"
// 输出：true
// 实例3：[["A","B","C","E"],["S","F","C","S"],["A","D","E","E"]], "ABCB"
// 输出：false

class Solution {
private:
    int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}; // 右、左、下、上四个方向
    
    bool dfs(vector<vector<char>>& grid, string& target, int index, int row, int col, vector<vector<bool>>& visited) {
        // 如果已经匹配完所有字符，返回true
        if (index == target.length()) {
            return true;
        }
        
        // 检查边界和访问状态
        if (row < 0 || row >= grid.size() || col < 0 || col >= grid[0].size() || 
            visited[row][col] || grid[row][col] != target[index]) {
            return false;
        }
        
        // 标记当前位置为已访问
        visited[row][col] = true;
        
        // 向四个方向递归搜索
        for (int i = 0; i < 4; i++) {
            int newRow = row + directions[i][0];
            int newCol = col + directions[i][1];
            
            if (dfs(grid, target, index + 1, newRow, newCol, visited)) {
                return true;
            }
        }
        
        // 回溯：取消标记当前位置
        visited[row][col] = false;
        return false;
    }
    
public:
    bool exist(vector<vector<char>>& grid, string target) {
        if (grid.empty() || grid[0].empty() || target.empty()) {
            return false;
        }
        
        int m = grid.size();
        int n = grid[0].size();
        vector<vector<bool>> visited(m, vector<bool>(n, false));
        
        // 从每个位置开始尝试搜索
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (dfs(grid, target, 0, i, j, visited)) {
                    return true;
                }
            }
        }
        
        return false;
    }
};

int main() {
    Solution solution;
    
    // 测试用例1
    vector<vector<char>> grid1 = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'}
    };
    string target1 = "ABCCED";
    cout << "测试1: " << (solution.exist(grid1, target1) ? "true" : "false") << endl;
    
    // 测试用例2
    vector<vector<char>> grid2 = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'}
    };
    string target2 = "SEE";
    cout << "测试2: " << (solution.exist(grid2, target2) ? "true" : "false") << endl;
    
    // 测试用例3
    vector<vector<char>> grid3 = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'}
    };
    string target3 = "ABCB";
    cout << "测试3: " << (solution.exist(grid3, target3) ? "true" : "false") << endl;
    
    return 0;
}