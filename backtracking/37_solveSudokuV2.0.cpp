#include <vector>
#include <set>

using namespace std;
class Solution {
private:
bool check(int row, int col, vector<vector<char>>& board, char val) {
    set<char> row_set(board[row].begin(), board[row].end());
    if (row_set.find(val) != row_set.end()) return false;
    set<char> col_set;
    for (int i = 0; i < board.size(); i++) {
        col_set.insert(board[i][col]);
    }
    if (col_set.find(val) != col_set.end()) return false;
    set<char> dist_set;
    int start_row = row / 3 * 3;
    int start_col = col / 3 * 3;
    for (int i = start_row; i < start_row + 3; i++) {
        for (int j = start_col; j < start_col + 3; j++) {
            dist_set.insert(board[i][j]);
        }
    }
    if (dist_set.find(val) != dist_set.end()) return false;
    return true;
}

bool isValid(int row, int col, char val, vector<vector<char>>& board) {
    for (int i = 0; i < 9; i++) { // 判断行里是否重复
        if (board[row][i] == val) {
            return false;
        }
    }
    for (int j = 0; j < 9; j++) { // 判断列里是否重复
        if (board[j][col] == val) {
            return false;
        }
    }
    int startRow = (row / 3) * 3;
    int startCol = (col / 3) * 3; 
    for (int i = startRow; i < startRow + 3; i++) { // 判断9方格里是否重复
        for (int j = startCol; j < startCol + 3; j++) {
            if (board[i][j] == val ) {
                return false;
            }
        }
    }
    return true;
}

bool backtracking(vector<vector<char>>& board) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == '.') {
                for (char c = '1'; c <= '9'; c++) {
                    if (isValid(i, j, c, board)) {
                        board[i][j] = c;
                        if (backtracking(board)) return true;
                        board[i][j] = '.';
                    }
                }
                return false; // 该格无解，回溯
            }
        }
    }
    return true; // 全部填满
}
public:
    void solveSudoku(vector<vector<char>> &board) {
        bool found = backtracking(board);
    }
};