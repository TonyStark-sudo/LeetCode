#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;

// random number array


// vector<double> creatnumsBycv() {
//     vector<double> nums(10);
//     cv::RNG rng(static_cast<unsigned>(time(0)));

//     for (int i = 0; i < nums.size(); i++) {
//         nums.push_back(rng.uniform(0, 50));
//     }
    
//     for (double num : nums) {
//         cout << num << " " << endl;
//     }

//     return nums;
// };

vector<int> creatnums(int size, bool _auto) {
    vector<int> nums;

    if (_auto == 1) {
        srand(static_cast<unsigned>(time(0)));
        for (int i = 0; i < size; i++) {
            nums.push_back(rand() % 100);
        }
    }

    else {
        cout << "请输入" << size << "个整数" << endl;
        for (int i = 0; i < size; i++) {
            int num;
            cin >> num;
            nums.push_back(num);
        }
    }

    return nums;

};

vector<int> sortArray(vector<int> &nums) {
    int size = nums.size();
    int temp;
    for (int i = 0; i < size; i++) {
        // 外循环用来控制需要有多少圈比较
        for (int j = 0; j <= size - i; j++){
        // 内循环用来遍历数组
        if (nums[j] > nums[j + 1]) {
            
            temp = nums[j + 1];
            nums[j + 1] = nums[j];
            nums[j] = temp;
        }

        }
    
    }
    return nums;
};

void _sortArray(vector<int> &nums) {
    int size = nums.size();
    int temp;
    for (int i = 0; i < size; i++) {
        // 外循环用来控制需要有多少圈比较
        for (int j = 0; j <= size - i; j++){
        // 内循环用来遍历数组
        if (nums[j] > nums[j + 1]) {
            
            temp = nums[j + 1];
            nums[j + 1] = nums[j];
            nums[j] = temp;
        }

        }
    
    }
    // return nums;
};

int main(int argc, char *argv[])
{
    vector<int> nums;
    nums = creatnums(20, 1);

    for (int num : nums) {
        cout << num << " ";
    }
    cout << endl;
    // nums = sortArray(nums);
    sortArray(nums);
    for (int num : nums) {
        cout << num << " ";
    }
    /* code */
    return 0;
}
