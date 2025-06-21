#include <vector>
#include <set>

using namespace std;
class Solution {
private:

    bool check(vector<vector<char>>& board, int row, int col, int num) {
        if (board[row][col] != '.') return false;
        // 判断第Row行有无重复数字
        set<char> row_set(board[row].begin(), board[row].end());
        if (row_set.find(char('0' + num)) != row_set.end()) return false;
        // 判断第Col列有无重复数字
        set<char> col_set;
        for (int i = 0; i < 9; i++) {
            col_set.insert(board[i][col]);
        }
        if (col_set.find(char('0' + num)) != col_set.end()) return false;
        // 如何判断区域有无重复数字
        set<char> dist_set;
        int start_row = row / 3 * 3;
        int start_col = col / 3 * 3;
        for (int i = start_row; i < start_row + 3; i++) {
            for (int j = start_col; j < start_col + 3; j++) {
                dist_set.insert(board[i][j]);
            }
        }
        if (dist_set.find(char('0' + num)) != dist_set.end()) return false;
        return true;
    }
    void backtracking(vector<vector<char>>& board, int Row, int Col) {
        if (Row == 8 && Col == 9) {
            return ;
        }
        for (int i = 1; i < 10; i++) {
            if (check(board, Row, Col, i)) {
                board[Row][Col] = char('0' + i);
            }
            for (int i = Row; i < board.size(); i++) {
                for (int j = Col + 1; j < board.size(); j++) {
                    if (board[i][j] == '.') {
                        backtracking(board, i, j);
                    }
                }
            }
            
        }
    }
public: 
    void solveSudoku(vector<vector<char>>& board) {
        int row = 10, col = 10;
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board.size(); j++) {
                if (board[i][j] == '.') {
                    row = i;
                    col = j;
                    break;
                }
            }
            if (row != 10 && col != 10) break;
        }
        backtracking(board, row, col);
    }
};