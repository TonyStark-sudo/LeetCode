#include <memory>
#include <vector>
#include <iostream>

class Car {
public:
    Car() { std::cout << "Car created" << std::endl; }
    ~Car() { std::cout << "Car destoryed\n"; }
    void drive() {}
};

int main(int argc, char const *argv[])
{
    auto vec_ptr = std::make_shared<std::vector<int>>(std::vector<int>{1, 2, 3, 4});
    // std::cout << vec_ptr->at(1) << std::endl;
    /*
    1. 多个 shared_ptr 可以指向同一个对象
    2. 引用计数：内部维护一个引用计数器，记录有多少个 shared_ptr 指向同一个对象
    3. 自动释放：当最后一个 shared_ptr 被销毁时，对象才被释放
    4. 线程安全：引用计数的增减操作是线程安全的
    5. 支持拷贝构造和赋值操作
    */
    std::shared_ptr<Car> car_shared_ptr = std::make_shared<Car>();
    // std::shared_ptr<Car> car_ptr = std::make_shared<Car>(Car());
    /*
    1. 只有一个 unique_ptr 可以拥有对象
    2. 不支持拷贝构造和拷贝赋值
    3. 支持移动语义 (std::move)
    4. 没有引用计数开销，性能更好
    */
    std::unique_ptr<Car> car_unique_ptr = std::make_unique<Car>();
    // Car* car_pointer = new Car();
    // delete car_pointer;
    return 0;
}
