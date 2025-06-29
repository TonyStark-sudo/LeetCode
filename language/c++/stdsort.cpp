#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

bool cmp(int a, int b) {
    if (a % 2 && !(b % 2)) return true;
    if (!(a % 2) && b % 2) return false;
    return  a > b;
}

 int main(int argc, char const *argv[])
 {
    vector<int> nums = {2, 1, 6, 3, 4, 5, 8, 9, 7};
    std::sort(nums.begin(), nums.end(), cmp);
    for (int& num : nums) {
        std::cout << num << " " << std::endl;
    }
    return 0;
}