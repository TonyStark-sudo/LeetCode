#include <vector>
#include <iostream>

using namespace std;

void printNums(const char* prefix, const vector<int>& nums) {
    cout << prefix << "[ ";
    for (int num : nums) {
        cout << num << ", ";
    }
    cout << "]" << endl;
}

void radixSort(vector<int>& nums) {
    int maxV = *max_element(nums.begin(), nums.end());
    int base = 1;
    while (base < maxV) {
        vector<vector<int>> buckets(10);
        for (int& num : nums) {
            buckets[num / base % 10].push_back(num);
        }
        vector<int> result;
        for (size_t i{0UL}; i < buckets.size(); ++i) {
            for (size_t j{0UL}; j < buckets[i].size(); ++j) {
                result.push_back(buckets[i][j]);
            }
        }
        nums = result;
        base *= 10;
    }
}

int main(int argc, char const *argv[])
{
    vector<int> nums{3121, 897, 3122, 3, 23, 5, 55, 7, 97, 123, 456, 1327};
    printNums("Before Sort: ", nums);
    radixSort(nums);
    printNums("After Sort: ", nums);
    return 0;
}