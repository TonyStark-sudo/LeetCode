#include <iostream>
#include <vector>

using namespace std;

int capacity;
int itemNum;
int sumWeight = 0;
int sumValue = 0;
int maxValue = 0;
void backtracking(int* weight, int* value, int startIndex) {
    if (sumWeight <= capacity) {
        maxValue = max(maxValue, sumValue);
    }
    if (sumWeight >= capacity) {
        return ;
    }
    for (int i = startIndex; i < itemNum; i++) {
        sumValue += value[i];
        sumWeight += weight[i];
        backtracking(weight, value, i + 1);
        sumWeight -= weight[i];
        sumValue -= value[i];
    }
}

int main(int argc, char const *argv[])
{
    cin >> itemNum >> capacity;
    int weight[itemNum], value[itemNum];
    for (int i = 0; i < itemNum; i++) {
        cin >> weight[i];
    }
    for (int i = 0; i < itemNum; i++) {
        cin >> value[i];
    }

    backtracking(weight, value, 0);
    // cout << "max value: " << maxValue << endl;
    cout << maxValue << endl;
    return 0;
}