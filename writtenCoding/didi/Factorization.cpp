#include <iostream>
#include <vector>

using namespace std;
vector<vector<int>> result;
vector<int> path;
int factorize_num = 1;
void backtracking(int n) {
    if (n <= 1) return ;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            factorize_num++;
            backtracking(n / i);
        }
    }
}
int main(int argc, char const *argv[])
{
    int num;
    cout << "Enter a number: ";
    cin >> num;
    backtracking(num);
    cout << "factorize num: " << factorize_num << endl;
    return 0;
}

