#include <iostream>
#include <vector>

using namespace std;

int M, N;
int main(int argc, char const *argv[])
{
    cin >> M >> N;
    vector<int> weights(M), value(M);
    for (int i = 0; i < M; i++) {
        cin >> weights[i];
    }
    for (int i = 0; i < M; i++) {
        cin >> value[i];
    }
    vector<vector<int>> dp(M, vector<int>(N + 1, 0));
    for (int i = 0; i <= N; i++) {
        if (i >= weights[0]) dp[0][i] = value[0];
    }
    for (int i = 1; i < M; i++) {
        for (int j = 1; j <= N; j++) {
            if (j - weights[i] >= 0) {
                dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - weights[i]] + value[i]);
            }
            else {
                dp[i][j] = dp[i - 1][j];
            }
            
        }
    }
    cout << dp[M - 1][N] << endl;
    return 0;
}
