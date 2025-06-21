#include <iostream>

int* func_stack() {
    int a = 10;
    return &a;
}

int* func_heap() {
    int* a = new int(10);
    return a;
}

int main(int argc, char const *argv[])
{
    int* p = func_stack();
    std::cout << "p point to: " << *p << std::endl;
    std::cout << "p point to: " << *p << std::endl;
    int* q = func_heap();
    std::cout << "q point to: " << *q << std::endl;
    std::cout << "q point to: " << *q << std::endl;
    std::cin.get();
    return 0;
}
