#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>
#include <cstdint>

// 基类
class Base {
public:
    virtual ~Base() {}
    virtual void show() { std::cout << "Base class" << std::endl; }
};

// 派生类
class Derived : public Base {
public:
    void show() override { std::cout << "Derived class" << std::endl; }
    void derivedMethod() { std::cout << "Derived specific method" << std::endl; }
};

// 另一个不相关的类
class Other {
public:
    void otherMethod() { std::cout << "Other class method" << std::endl; }
};

int main() {
    std::cout << "=== C++ 类型转换示例 ===\n" << std::endl;
    
    // ========== 1. static_cast ==========
    std::cout << "1. static_cast (静态转换)" << std::endl;
    
    // 基本类型转换
    double d = 3.14159;
    int i = static_cast<int>(d);
    std::cout << "double " << d << " -> int " << i << std::endl;
    
    // 类层次结构中的转换（向上转换）
    Derived derived;
    Base* basePtr = static_cast<Base*>(&derived);
    basePtr->show();
    
    // 向下转换（不安全，但编译时允许）
    Base base;
    Base* basePtrToBase = &base;
    Derived* derivedPtr = static_cast<Derived*>(basePtrToBase); // 危险！
    // derivedPtr->derivedMethod(); // 可能导致未定义行为
    
    std::cout << std::endl;
    
    // ========== 2. dynamic_cast ==========
    std::cout << "2. dynamic_cast (动态转换)" << std::endl;
    
    Base* basePtr1 = new Derived(); // 实际指向Derived对象
    Base* basePtr2 = new Base();    // 实际指向Base对象
    
    // 安全的向下转换
    Derived* derivedPtr1 = dynamic_cast<Derived*>(basePtr1);
    if (derivedPtr1) {
        std::cout << "成功转换为Derived*" << std::endl;
        derivedPtr1->derivedMethod();
    } else {
        std::cout << "转换失败" << std::endl;
    }
    
    // 失败的向下转换
    Derived* derivedPtr2 = dynamic_cast<Derived*>(basePtr2);
    if (derivedPtr2) {
        std::cout << "成功转换为Derived*" << std::endl;
    } else {
        std::cout << "转换失败 - basePtr2实际指向Base对象" << std::endl;
    }
    
    // 引用形式的dynamic_cast
    try {
        Derived& derivedRef = dynamic_cast<Derived&>(*basePtr2);
        derivedRef.derivedMethod();
    } catch (std::bad_cast& e) {
        std::cout << "引用转换失败，抛出异常: " << e.what() << std::endl;
    }
    
    delete basePtr1;
    delete basePtr2;
    std::cout << std::endl;
    
    // ========== 3. const_cast ==========
    std::cout << "3. const_cast (常量性转换)" << std::endl;
    
    const int constValue = 42;
    const int* constPtr = &constValue;
    
    // 移除const属性
    int* nonConstPtr = const_cast<int*>(constPtr);
    std::cout << "原始const值: " << *constPtr << std::endl;
    std::cout << "通过非const指针访问: " << *nonConstPtr << std::endl;
    
    // 注意：修改原本const的变量是未定义行为
    // *nonConstPtr = 100; // 危险！不要这样做
    
    // 更安全的用法：移除函数参数的const
    int normalValue = 100;
    const int* constPtrToNormal = &normalValue;
    int* modifiablePtr = const_cast<int*>(constPtrToNormal);
    *modifiablePtr = 200; // 这是安全的，因为原始变量不是const
    std::cout << "修改后的值: " << normalValue << std::endl;
    
    std::cout << std::endl;
    
    // ========== 4. reinterpret_cast ==========
    std::cout << "4. reinterpret_cast (重新解释转换)" << std::endl;
    
    int number = 0x12345678;
    
    // 将int*重新解释为char*
    char* charPtr = reinterpret_cast<char*>(&number);
    std::cout << "int值: " << std::hex << number << std::endl;
    std::cout << "作为字节序列: ";
    for (size_t j = 0; j < sizeof(int); ++j) {
        std::cout << std::hex << static_cast<unsigned char>(charPtr[j]) << " ";
    }
    std::cout << std::dec << std::endl;
    
    // 指针和整数之间的转换
    uintptr_t address = reinterpret_cast<uintptr_t>(&number);
    std::cout << "指针地址: " << std::hex << address << std::dec << std::endl;
    
    // 不相关类型之间的转换（危险）
    Base baseObj;
    Other* otherPtr = reinterpret_cast<Other*>(&baseObj);
    // otherPtr->otherMethod(); // 危险！可能导致崩溃
    
    std::cout << std::endl;
    
    // ========== 5. C风格转换 ==========
    std::cout << "5. C风格转换 (不推荐)" << std::endl;
    
    double d2 = 3.7;
    int i2 = (int)d2; // C风格转换
    std::cout << "C风格转换: " << d2 << " -> " << i2 << std::endl;
    
    // C风格转换的问题：不够明确，可能隐藏错误
    const int* constPtr2 = &constValue;
    int* ptr2 = (int*)constPtr2; // 相当于const_cast + static_cast
    
    std::cout << std::endl;
    
    // ========== 6. 隐式转换 ==========
    std::cout << "6. 隐式转换" << std::endl;
    
    int intVal = 42;
    double doubleVal = intVal; // 隐式转换
    std::cout << "隐式转换: int " << intVal << " -> double " << doubleVal << std::endl;
    
    // 类类型的隐式转换
    std::string str = "Hello"; // 从const char*隐式转换为string
    std::cout << "字符串隐式转换: " << str << std::endl;
    
    
    std::cout << "\n=== 四种类型转换的具体使用场景 ===\n" << std::endl;
    
    // ========== static_cast 使用场景 ==========
    std::cout << "📌 static_cast 使用场景:" << std::endl;
    
    // 场景1: 数值类型转换
    std::cout << "1. 数值类型转换 - 精度控制" << std::endl;
    double price = 19.99;
    int dollars = static_cast<int>(price);
    std::cout << "价格: $" << price << " -> 整数部分: $" << dollars << std::endl;
    
    // 场景2: enum与整数转换
    enum Color { RED = 0, GREEN = 1, BLUE = 2 };
    int colorValue = 1;
    Color selectedColor = static_cast<Color>(colorValue);
    std::cout << "枚举转换: " << colorValue << " -> Color(" << selectedColor << ")" << std::endl;
    
    // 场景3: void*转换为具体类型（需要确保类型正确）
    int value = 42;
    void* voidPtr = &value;
    int* intPtr = static_cast<int*>(voidPtr);
    std::cout << "void*转换: " << *intPtr << std::endl;
    
    std::cout << std::endl;
    
    // ========== dynamic_cast 使用场景 ==========
    std::cout << "📌 dynamic_cast 使用场景:" << std::endl;
    
    // 场景1: 多态容器中的类型检查
    std::cout << "1. 多态容器处理" << std::endl;
    std::vector<Base*> objects;
    objects.push_back(new Derived());
    objects.push_back(new Base());
    
    for (size_t k = 0; k < objects.size(); ++k) {
        std::cout << "对象 " << k << ": ";
        if (Derived* derived = dynamic_cast<Derived*>(objects[k])) {
            std::cout << "这是Derived对象";
            derived->derivedMethod();
        } else {
            std::cout << "这是Base对象" << std::endl;
        }
    }
    
    // 清理内存
    for (auto* obj : objects) {
        delete obj;
    }
    
    // 场景2: 访问者模式实现
    std::cout << "\n2. 类型安全的向下转换" << std::endl;
    Base* unknownObject = new Derived();
    
    // 尝试转换为Derived类型
    if (auto* derivedObj = dynamic_cast<Derived*>(unknownObject)) {
        std::cout << "成功识别为Derived对象，可以安全调用特有方法" << std::endl;
        derivedObj->derivedMethod();
    }
    
    delete unknownObject;
    std::cout << std::endl;
    
    // ========== const_cast 使用场景 ==========
    std::cout << "📌 const_cast 使用场景:" << std::endl;
    
    // 场景1: 与C库函数交互
    std::cout << "1. 与C库函数交互" << std::endl;
    const char* filename = "data.txt";
    // 某些C库函数可能需要非const char*，但实际不修改内容
    char* mutableFilename = const_cast<char*>(filename);
    std::cout << "文件名: " << mutableFilename << " (仅用于传递给C函数)" << std::endl;
    
    // 场景2: 在const成员函数中修改mutable成员
    class Cache {
        mutable int accessCount = 0;  // mutable允许在const函数中修改
    public:
        int getValue() const {
            // 在const函数中修改mutable成员
            const_cast<Cache*>(this)->accessCount++;
            std::cout << "缓存访问次数: " << accessCount << std::endl;
            return 100;
        }
    };
    
    const Cache cache;
    cache.getValue();
    cache.getValue();
    
    // 场景3: 重载const和非const版本的函数
    class DataContainer {
        std::string data = "Hello World";
    public:
        // const版本
        const std::string& getData() const {
            std::cout << "调用const版本的getData" << std::endl;
            return data;
        }
        
        // 非const版本，复用const版本的实现
        std::string& getData() {
            std::cout << "调用非const版本的getData" << std::endl;
            return const_cast<std::string&>(
                static_cast<const DataContainer*>(this)->getData()
            );
        }
    };
    
    DataContainer container;
    const DataContainer& constContainer = container;
    
    constContainer.getData();    // 调用const版本
    container.getData();         // 调用非const版本
    
    std::cout << std::endl;
    
    // ========== reinterpret_cast 使用场景 ==========
    std::cout << "📌 reinterpret_cast 使用场景:" << std::endl;
    
    // 场景1: 网络编程 - 字节序转换
    std::cout << "1. 网络编程 - 数据序列化" << std::endl;
    struct NetworkPacket {
        uint32_t id = 0x12345678;
        uint16_t size = 1024;
        uint16_t flags = 0xABCD;
    } packet;
    
    // 将结构体转换为字节数组进行网络传输
    const char* packetBytes = reinterpret_cast<const char*>(&packet);
    std::cout << "数据包字节表示: ";
    for (size_t i = 0; i < sizeof(NetworkPacket); ++i) {
        std::cout << std::hex << static_cast<unsigned char>(packetBytes[i]) << " ";
    }
    std::cout << std::dec << std::endl;
    
    // 场景2: 内存对齐和优化
    std::cout << "\n2. 内存对齐检查" << std::endl;
    int alignedData[4] = {1, 2, 3, 4};
    uintptr_t address1 = reinterpret_cast<uintptr_t>(&alignedData[0]);
    uintptr_t address2 = reinterpret_cast<uintptr_t>(&alignedData[1]);
    
    std::cout << "数组元素地址差: " << (address2 - address1) << " 字节" << std::endl;
    std::cout << "是否4字节对齐: " << (address1 % 4 == 0 ? "是" : "否") << std::endl;
    
    // 场景3: 硬件寄存器访问（嵌入式编程）
    std::cout << "\n3. 模拟硬件寄存器访问" << std::endl;
    const uintptr_t REGISTER_ADDRESS = 0x40000000; // 模拟硬件地址
    // 注意：在实际嵌入式系统中才这样使用
    // volatile uint32_t* hardwareRegister = reinterpret_cast<volatile uint32_t*>(REGISTER_ADDRESS);
    std::cout << "硬件寄存器地址: 0x" << std::hex << REGISTER_ADDRESS << std::dec << std::endl;
    
    // 场景4: 函数指针转换
    std::cout << "\n4. 函数指针存储和恢复" << std::endl;
    auto lambda = []() { std::cout << "Lambda函数被调用" << std::endl; };
    
    // 将函数指针转换为通用指针存储
    void* functionPtr = reinterpret_cast<void*>(&lambda);
    
    // 恢复并调用（需要确保类型匹配）
    auto* restoredLambda = reinterpret_cast<decltype(lambda)*>(functionPtr);
    (*restoredLambda)();
    
    std::cout << "\n=== 类型转换使用建议 ===\n" << std::endl;
    
    std::cout << "🎯 static_cast: 安全的编译时转换，用于相关类型间转换" << std::endl;
    std::cout << "   - 数值类型转换、枚举转换、类层次向上转换" << std::endl;
    
    std::cout << "🎯 dynamic_cast: 运行时类型检查，用于多态类型安全转换" << std::endl;
    std::cout << "   - 向下转换、侧向转换、类型识别" << std::endl;
    
    std::cout << "🎯 const_cast: const属性修改，谨慎使用" << std::endl;
    std::cout << "   - 与C代码交互、实现const重载、mutable成员访问" << std::endl;
    
    std::cout << "🎯 reinterpret_cast: 底层位重新解释，最危险" << std::endl;
    std::cout << "   - 网络编程、硬件访问、内存操作、性能优化" << std::endl;
    
    return 0;
}