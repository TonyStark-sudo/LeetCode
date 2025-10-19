// 最大岛屿面积
// 给你一个大小为m * n的二进制矩阵grid。岛屿是由一些相邻的 1 (代表陆地) 构成的组合，
// 这里的「相邻」要求两个1必须在水平或者垂直方向上相邻。你可以假设grid的四个边缘都被0（代表水域）包围着。
// 岛屿的面积是岛上值为1的单元格的数目。计算并返回grid中最大的岛屿面积。如果没有岛屿，则返回面积为0。

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Solution {
private:
    int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}; // 右、左、下、上四个方向
    
    int dfs(vector<vector<int>>& grid, int row, int col) {
        // 边界检查或当前位置不是陆地
        if (row < 0 || row >= grid.size() || col < 0 || col >= grid[0].size() || grid[row][col] == 0) {
            return 0;
        }
        
        // 标记当前位置为已访问（设为0，避免重复访问）
        grid[row][col] = 0;
        int area = 1; // 当前位置贡献面积1
        
        // 向四个方向递归搜索
        for (int i = 0; i < 4; i++) {
            int newRow = row + directions[i][0];
            int newCol = col + directions[i][1];
            area += dfs(grid, newRow, newCol);
        }
        
        return area;
    }
    
public:
    int maxAreaOfIsland(vector<vector<int>>& grid) {
        if (grid.empty() || grid[0].empty()) {
            return 0;
        }
        
        int maxArea = 0;
        int m = grid.size();
        int n = grid[0].size();
        
        // 遍历每个位置
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 1) {
                    // 找到陆地，开始DFS计算面积
                    int currentArea = dfs(grid, i, j);
                    maxArea = max(maxArea, currentArea);
                }
            }
        }
        
        return maxArea;
    }
};

int main() {
    Solution solution;
    
    // 测试用例1
    vector<vector<int>> grid1 = {
        {1, 1, 0, 0, 0},
        {1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1},
        {0, 0, 0, 1, 1}
    };
    cout << "测试1最大岛屿面积: " << solution.maxAreaOfIsland(grid1) << endl;
    
    // 测试用例2
    vector<vector<int>> grid2 = {
        {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0},
        {0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0}
    };
    cout << "测试2最大岛屿面积: " << solution.maxAreaOfIsland(grid2) << endl;
    
    // 测试用例3（全为0的情况）
    vector<vector<int>> grid3 = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };
    cout << "测试3最大岛屿面积: " << solution.maxAreaOfIsland(grid3) << endl;
    
    return 0;
}