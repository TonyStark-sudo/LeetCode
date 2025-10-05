#include <string>
#include <vector>

using namespace std;

class MySolution {
private:
    vector<vector<bool>> visited;
    int dir[4][2] = {0, 1, 1, 0, 0, -1, -1, 0};
    string path;
    vector<string> paths;
    bool hasWord = false;
    void init(const vector<vector<char>>& board) {
        visited = vector<vector<bool>>(board.size(), vector<bool>(board[0].size(), false));
    }
    void dfs(const vector<vector<char>>& board, string word, int x, int y, vector<vector<bool>> visited, int index) {
        if (visited[x][y]) return ;
        if (index == word.size()) {
            hasWord = true;
            return ;
        }
        visited[x][y] = true;
        for (int i = 0; i < 4; i++) {
            int nx = x + dir[i][0];
            int ny = y + dir[i][1];
            if (nx < 0 || ny < 0 || nx >= board.size() || ny >= board[0].size()) {
                continue;
            }
            if (board[nx][ny] == word[index] && !visited[nx][ny]) {
                dfs(board, word, nx, ny, visited, index + 1);
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
                if (hasWord) break;
            }
        }
        return hasWord;
    }
};

class Solution {
public:
    bool check(vector<vector<char>>& board, vector<vector<int>>& visited, int i, int j, string& s, int k) {
        if (board[i][j] != s[k]) {
            return false;
        } else if (k == s.length() - 1) {
            return true;
        }
        visited[i][j] = true;
        vector<pair<int, int>> directions{{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
        bool result = false;
        for (const auto& dir: directions) {
            int newi = i + dir.first, newj = j + dir.second;
            if (newi >= 0 && newi < board.size() && newj >= 0 && newj < board[0].size()) {
                if (!visited[newi][newj]) {
                    bool flag = check(board, visited, newi, newj, s, k + 1);
                    if (flag) {
                        result = true;
                        break;
                    }
                }
            }
        }
        visited[i][j] = false;
        return result;
    }

    bool exist(vector<vector<char>>& board, string word) {
        int h = board.size(), w = board[0].size();
        vector<vector<int>> visited(h, vector<int>(w));
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                bool flag = check(board, visited, i, j, word, 0);
                if (flag) {
                    return true;
                }
            }
        }
        return false;
    }
};