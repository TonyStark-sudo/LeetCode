#include <iostream>

int a_g = 1; // 全局变量，数据段
int b_g = 2; // 全局变量，数据段

const int a_const_g = 1; // 全局常量，常量区（只读数据段）
const int b_const_g = 2; // 全局常量，常量区（只读数据段）

static const int a_const_static_g = 1; // 静态全局常量，常量区（只读数据段）
static const int b_const_static_g = 2; // 静态全局常量，常量区（只读数据段）

int main() {
    int a = 1; // 局部变量，栈
    int b = 2; // 局部变量，栈
    {
        a = 11;
        b = 22;
    }
    int* a_heap = nullptr;
    int* b_heap = nullptr;
    {
        a_heap = new int(1);
        b_heap = new int(2);
    }
    std::cout << "a: " << a << std::endl;
    std::cout << "b: " << b << std::endl;
    std::cout << "a_heap: " << *a_heap << std::endl;
    std::cout << "b_heap: " << *b_heap << std::endl;
    const int a_const = 1; // 局部常量，栈
    const int b_const = 2; // 局部常量，栈
    static int a_s = 1; // 静态局部变量，数据段
    static int b_s = 2; // 静态局部变量，数据段

    std::cout << "a_g address: " << &a_g << " (全局变量，数据段)" << std::endl;
    std::cout << "b_g address: " << &b_g << " (全局变量，数据段)" << std::endl;
    std::cout << "a_const_g address: " << &a_const_g << " (全局常量，常量区)" << std::endl;
    std::cout << "b_const_g address: " << &b_const_g << " (全局常量，常量区)" << std::endl;
    std::cout << "a_heap address: " << a_heap << " (堆变量)" << std::endl;
    std::cout << "b_heap address: " << b_heap << " (堆变量)" << std::endl;
    std::cout << "a_const_static_g address: " << &a_const_static_g << " (静态全局常量，常量区)" << std::endl;
    std::cout << "b_const_static_g address: " << &b_const_static_g << " (静态全局常量，常量区)" << std::endl;
    std::cout << "a address: " << &a << " (局部变量，栈)" << std::endl;
    std::cout << "b address: " << &b << " (局部变量，栈)" << std::endl;
    std::cout << "a_const address: " << &a_const << " (局部常量，栈)" << std::endl;
    std::cout << "b_const address: " << &b_const << " (局部常量，栈)" << std::endl;
    std::cout << "a_s address: " << &a_s << " (静态局部变量，数据段)" << std::endl;
    std::cout << "b_s address: " << &b_s << " (静态局部变量，数据段)" << std::endl;

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get(); // macOS 下用这个暂停
    delete a_heap;
    delete b_heap;
    return 0;
}