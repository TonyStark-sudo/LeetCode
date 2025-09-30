#include <string>
#include <vector>

using namespace std;

class Solution {
private:
    vector<vector<bool>> visited;
    int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};
    string path;
    vector<string> paths;
    bool hasWord = false;
    void init(const vector<vector<char>>& board) {
        visited = vector<vector<bool>>(board.size(), vector<bool>(board[0].size(), false));
    }
    void dfs(const vector<vector<char>>& board, string word, int x, int y, vector<vector<bool>>& visited, int& index) {
        if (visited[x][y]) return ;
        if (index == word.size()) {
            hasWord = true;
            return ;
        }
        visited[x][y] = true;
        for (int i = 0; i < 4; i++) {
            int nx = x + dir[i][0];
            int ny = y + dir[i][1];
            if (nx < 0 || nx < 0 || nx >= board.size() || ny >= board[0].size()) {
                continue;
            }
            if (board[nx][ny] == word[index] && !visited[nx][ny]) {
                dfs(board, word, nx, ny, visited, ++index);
            }
        }
    }
public:
    bool exist(vector<vector<char>>& board, string word) {
        init(board);
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board[0].size(); j++) {
                if (board[i][j] == word[0]) {
                    visited = vector<vector<bool>>(board.size(), vector<bool>(board[0].size(), false));
                    int index = 1;
                    dfs(board, word, i, j, visited, index);
                }
            }
        }
        return hasWord;
    }
};