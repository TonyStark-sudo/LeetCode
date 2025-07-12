#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    // 初始化vector容器
    vector<int> vec{1, 2, 3, 4, 5};
    vector<int> nums(12, 0);
    vector<int> v1 = {1, 2, 3, 4, 5, 6};

    // C++数组的初始化方式
    int a[6] = {1, 2, 3, 4, 5, 6};
    // std::cout << "vec[6] = " << vec[6] << std::endl;
    // 异常
    // std::cout << "vec.at(6) = " << vec.at(6) << std::endl;
    std::cout << "vec size = " << vec.size() << std::endl;
    std::cout << "vec capa = " << vec.capacity() << std::endl;
    return 0;
}
