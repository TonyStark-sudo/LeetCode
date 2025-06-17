#include <vector>
#include <iostream>
#include <algorithm>

int main(int argc, char const *argv[])
{
    std::vector<int> nums = {1, 3, 2, 4, 5, 1, 3};
    int even_count = std::count_if(nums.begin(), nums.end(), [](int x) { return x % 2 == 0; });
    for (auto& num : nums) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    std::cout << "even in nums: " << even_count << std::endl;
    return 0;
}
