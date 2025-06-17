#include <memory>
#include <vector>
#include <iostream>

int main(int argc, char const *argv[])
{
    auto vec_ptr = std::make_shared<std::vector<int>>(std::vector<int>{1, 2, 3, 4});
    std::cout << vec_ptr->at(1) << std::endl;
    return 0;
}
