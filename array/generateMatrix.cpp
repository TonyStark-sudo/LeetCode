#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
void printMatrix(const vector<vector<int>>& nums) {
    for (const auto& row : nums) { // 遍历每一行
        for (const auto& val : row) { // 遍历当前行的每个元素
            cout << val << " "; // 输出元素值并用空格分隔
        }
        cout << endl; // 换行以分隔不同的行
    }
}

class Solution{
    public:
    vector<vector<int>> generateMatrix(int n) {
        vector<vector<int>> nums(n, vector<int>(n, 0));
        int start_x = 0, start_y = 0, offset = 1, cnt = 1;
        int i, j;
        for (int k = 0; k < n / 2; k++){
            for (j = start_y; j < n - offset; j++)
                nums[start_x][j] = cnt++;
            for (i = start_x; i < n - offset; i++)
                nums[i][j] = cnt++; 
            for ( ; j > start_y; j--)
                nums[i][j] = cnt++;
            for ( ; i > start_x; i--)
                nums[i][j] = cnt++;

            start_x++;
            start_y++;

            offset++;

        }

        if (n % 2) {
            nums[n / 2][n / 2] = cnt;
        }

        return nums;
    }
    
};

int main(int argc, char *argv[])
{
    Solution soluton;
    vector<vector<int>> result;
    result = soluton.generateMatrix(17);
    printMatrix(result);
    return 0;
}
