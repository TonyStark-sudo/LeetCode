#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char const *argv[])
{
    int n, m;
    cin >> n >> m;
    vector<int> weight(n), value(n);
    for (int i = 0; i < n; i++) {
        cin >> weight[i];
    }
    for (int i = 0; i < n; i++) {
        cin >> value[i];
    }
    vector<int> dp(m + 1, 0);
    // for (int j = 0; j <= m; j++) {
    //     if (j >= weight[0]) dp[j] = value[0];
    // }
    for (int i = 0; i < n; i++) {
        for (int j = m; j >= weight[i]; j--) {
            // if (j - weight[i] >= 0) {
            dp[j] = max(dp[j], dp[j - weight[i]] + value[i]);
            // }
            // else 
            // {
            //     dp[j] = dp[j];
            // }
        }
    }
    cout << dp[m] << endl;
    return 0;
}
