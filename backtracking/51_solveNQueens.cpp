#include <vector>
#include <string>

using namespace std;
class Solution {
private:
    vector<vector<string>> result;
    vector<string> chessboard;
    bool isValid(int row, int col, vector<string>& chessboard) {
        // 没有和[row, col]同行的皇后，不用遍历该行
        // for (int i = row, j = 0; j < chessboard.size(); j++) {
        //     if (j != col && chessboard[i][j] == 'Q') {
        //         return false;
        //     }
        // }
        for (int j = col, i = 0; i < chessboard.size(); i++) {
            if (i != row && chessboard[i][j] == 'Q') {
                return false;
            }
        }
        // [row, col]右下角一定没有皇后
        // for (int i = row + 1, j = col + 1; i < chessboard.size() && j < chessboard.size(); i++, j++) {
        //     if (chessboard[i][j] == 'Q') {
        //         return false;
        //     }
        // }
        for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
            if (chessboard[i][j] == 'Q') {
                return false;
            }
        }
        for (int i = row - 1, j = col + 1; i >= 0 && j < chessboard.size(); i--, j++) {
            if (chessboard[i][j] == 'Q') {
                return false;
            }
        }
        // [row, col]左下角也一定没有皇后
        // for (int i = row + 1, j = col - 1; i < chessboard.size() && j >= 0; i++, j--) {
        //     if (chessboard[i][j] == 'Q') {
        //         return false;
        //     }
        // }
        return true;
    }
    void backtracking(int n, int row, vector<string>& chessboard) {
        if (row == n) {
            result.push_back(chessboard);
            return ;
        }
        for (int col = 0; col < n; col++) {
            if (isValid(row, col, chessboard)) {
                chessboard[row][col] = 'Q';
                backtracking(n, row + 1, chessboard);
                chessboard[row][col] = '.';
            }
        }
    }
public:
    vector<vector<string>> solveNQueens(int n) {
        vector<string> chessboard(n, string(n, '.'));
        backtracking(n, 0, chessboard);
        return result;
    }
};