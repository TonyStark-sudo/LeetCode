#include <iostream>
#include <string>
#include <vector>

// ========== 1. 友元函数示例 ==========
class BankAccount {
private:
    std::string owner;
    double balance;
    std::string accountNumber;

public:
    BankAccount(const std::string& owner, double balance, const std::string& accNum)
        : owner(owner), balance(balance), accountNumber(accNum) {}
    
    // 友元函数声明 - 可以访问私有成员
    friend void displayAccount(const BankAccount& account);
    friend bool transfer(BankAccount& from, BankAccount& to, double amount);
    
    // 友元操作符重载
    friend std::ostream& operator<<(std::ostream& os, const BankAccount& account);
    friend BankAccount operator+(const BankAccount& acc1, const BankAccount& acc2);
    
    // 普通成员函数
    double getBalance() const { return balance; }
    std::string getOwner() const { return owner; }
};

// 友元函数实现 - 可以直接访问私有成员
void displayAccount(const BankAccount& account) {
    std::cout << "账户所有者: " << account.owner << std::endl;  // 直接访问私有成员
    std::cout << "账户余额: $" << account.balance << std::endl;
    std::cout << "账户号码: " << account.accountNumber << std::endl;
}

bool transfer(BankAccount& from, BankAccount& to, double amount) {
    if (from.balance >= amount) {  // 直接访问私有成员
        from.balance -= amount;
        to.balance += amount;
        return true;
    }
    return false;
}

// 友元操作符重载
std::ostream& operator<<(std::ostream& os, const BankAccount& account) {
    os << account.owner << "'s account: $" << account.balance;
    return os;
}

BankAccount operator+(const BankAccount& acc1, const BankAccount& acc2) {
    // 创建合并账户（示例用途）
    return BankAccount(acc1.owner + " & " + acc2.owner, 
                      acc1.balance + acc2.balance, 
                      acc1.accountNumber + "-" + acc2.accountNumber);
}

// ========== 2. 友元类示例 ==========
class Computer;  // 前向声明

class Processor {
private:
    std::string model;
    double clockSpeed;  // GHz
    
    // 私有函数
    void overclock() {
        clockSpeed *= 1.1;
        std::cout << "处理器超频到: " << clockSpeed << "GHz" << std::endl;
    }
    
public:
    Processor(const std::string& model, double speed) 
        : model(model), clockSpeed(speed) {}
    
    // 声明Computer为友元类
    friend class Computer;
    
    void displayInfo() const {
        std::cout << "处理器: " << model << " @ " << clockSpeed << "GHz" << std::endl;
    }
};

class Computer {
private:
    std::string brand;
    Processor cpu;
    
public:
    Computer(const std::string& brand, const Processor& processor)
        : brand(brand), cpu(processor) {}
    
    // 作为友元类，可以访问Processor的私有成员
    void optimizePerformance() {
        std::cout << "优化 " << brand << " 计算机性能..." << std::endl;
        
        // 直接访问Processor的私有成员
        std::cout << "当前CPU型号: " << cpu.model << std::endl;
        std::cout << "当前时钟频率: " << cpu.clockSpeed << "GHz" << std::endl;
        
        // 调用Processor的私有方法
        if (cpu.clockSpeed < 4.0) {
            cpu.overclock();
        }
    }
    
    void displaySpecs() const {
        std::cout << "品牌: " << brand << std::endl;
        cpu.displayInfo();
    }
};

// ========== 3. 相互友元示例 ==========
class Student;  // 前向声明

class Teacher {
private:
    std::string name;
    std::string subject;
    std::vector<int> gradeBook;  // 成绩册
    
public:
    Teacher(const std::string& name, const std::string& subject)
        : name(name), subject(subject) {}
    
    // 声明Student为友元
    friend class Student;
    
    void addGrade(int grade) {
        gradeBook.push_back(grade);
    }
    
    std::string getName() const { return name; }
    std::string getSubject() const { return subject; }
    
    // 友元函数用于访问学生私有数据
    void assignHomework(Student& student);
};

class Student {
private:
    std::string name;
    int studentId;
    std::vector<std::string> assignments;  // 作业列表
    
public:
    Student(const std::string& name, int id) : name(name), studentId(id) {}
    
    // 声明Teacher为友元
    friend class Teacher;
    
    std::string getName() const { return name; }
    int getId() const { return studentId; }
    
    void displayAssignments() const {
        std::cout << name << " 的作业列表:" << std::endl;
        for (size_t i = 0; i < assignments.size(); ++i) {
            std::cout << "  " << (i+1) << ". " << assignments[i] << std::endl;
        }
    }
    
    // 友元函数用于查看老师的成绩册（仅示例，实际中不建议）
    void checkTeacherGrades(const Teacher& teacher) {
        std::cout << teacher.name << " 老师的成绩册:" << std::endl;
        for (size_t i = 0; i < teacher.gradeBook.size(); ++i) {
            std::cout << "  成绩 " << (i+1) << ": " << teacher.gradeBook[i] << std::endl;
        }
    }
};

// Teacher的成员函数实现
void Teacher::assignHomework(Student& student) {
    std::string homework = subject + " 作业 #" + std::to_string(student.assignments.size() + 1);
    student.assignments.push_back(homework);  // 直接访问Student的私有成员
    std::cout << name << " 老师给 " << student.name << " 布置了: " << homework << std::endl;
}

// ========== 4. 友元模板示例 ==========
template<typename T>
class Matrix {
private:
    T** data;
    int rows, cols;
    
public:
    Matrix(int r, int c) : rows(r), cols(c) {
        data = new T*[rows];
        for (int i = 0; i < rows; ++i) {
            data[i] = new T[cols]();
        }
    }
    
    ~Matrix() {
        for (int i = 0; i < rows; ++i) {
            delete[] data[i];
        }
        delete[] data;
    }
    
    // 友元模板函数
    template<typename U>
    friend Matrix<U> multiply(const Matrix<U>& m1, const Matrix<U>& m2);
    
    void setValue(int r, int c, T value) {
        if (r >= 0 && r < rows && c >= 0 && c < cols) {
            data[r][c] = value;
        }
    }
    
    T getValue(int r, int c) const {
        return (r >= 0 && r < rows && c >= 0 && c < cols) ? data[r][c] : T();
    }
    
    void display() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                std::cout << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

// 友元模板函数实现
template<typename T>
Matrix<T> multiply(const Matrix<T>& m1, const Matrix<T>& m2) {
    if (m1.cols != m2.rows) {
        throw std::invalid_argument("矩阵维度不匹配");
    }
    
    Matrix<T> result(m1.rows, m2.cols);
    
    // 直接访问私有数据成员进行矩阵乘法
    for (int i = 0; i < m1.rows; ++i) {
        for (int j = 0; j < m2.cols; ++j) {
            for (int k = 0; k < m1.cols; ++k) {
                result.data[i][j] += m1.data[i][k] * m2.data[k][j];
            }
        }
    }
    
    return result;
}

// ========== 5. 友元与继承示例 ==========
class Shape {
protected:
    double area;
    
public:
    Shape() : area(0.0) {}
    virtual ~Shape() {}
    
    // 友元函数
    friend void debugShape(const Shape& shape);
    
    virtual void calculateArea() = 0;
    double getArea() const { return area; }
};

void debugShape(const Shape& shape) {
    std::cout << "Debug - 图形面积: " << shape.area << std::endl;
}

class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(double r) : radius(r) { calculateArea(); }
    
    void calculateArea() override {
        area = 3.14159 * radius * radius;
    }
    
    // 注意：友元关系不会被继承
    // 如果需要访问Circle的私有成员，需要重新声明友元
    friend void debugCircle(const Circle& circle);
};

void debugCircle(const Circle& circle) {
    std::cout << "Debug - 圆形半径: " << circle.radius << ", 面积: " << circle.area << std::endl;
}

int main() {
    std::cout << "=== C++ 友元（Friend）用法示例 ===\n" << std::endl;
    
    // ========== 1. 友元函数示例 ==========
    std::cout << "1. 友元函数示例" << std::endl;
    
    BankAccount alice("Alice", 1000.0, "ACC001");
    BankAccount bob("Bob", 500.0, "ACC002");
    
    std::cout << "转账前:" << std::endl;
    displayAccount(alice);  // 友元函数可以访问私有成员
    displayAccount(bob);
    
    if (transfer(alice, bob, 200.0)) {  // 友元函数操作两个对象的私有数据
        std::cout << "\n转账成功！转账后:" << std::endl;
    }
    
    // 使用友元操作符重载
    std::cout << alice << std::endl;  // 友元operator<<
    std::cout << bob << std::endl;
    
    // 友元operator+
    BankAccount joint = alice + bob;
    std::cout << "合并账户: " << joint << std::endl;
    
    std::cout << std::endl;
    
    // ========== 2. 友元类示例 ==========
    std::cout << "2. 友元类示例" << std::endl;
    
    Processor intel("Intel i7-12700K", 3.6);
    Computer myPC("Dell", intel);
    
    std::cout << "优化前:" << std::endl;
    myPC.displaySpecs();
    
    std::cout << "\n执行性能优化:" << std::endl;
    myPC.optimizePerformance();  // Computer作为友元类可以访问Processor私有成员
    
    std::cout << "\n优化后:" << std::endl;
    myPC.displaySpecs();
    
    std::cout << std::endl;
    
    // ========== 3. 相互友元示例 ==========
    std::cout << "3. 相互友元示例" << std::endl;
    
    Teacher mathTeacher("张老师", "数学");
    Student student1("小明", 12345);
    
    // 老师给学生布置作业（访问学生的私有数据）
    mathTeacher.assignHomework(student1);
    mathTeacher.assignHomework(student1);
    
    student1.displayAssignments();
    
    // 添加一些成绩
    mathTeacher.addGrade(95);
    mathTeacher.addGrade(87);
    mathTeacher.addGrade(92);
    
    std::cout << std::endl;
    // 学生查看老师成绩册（仅示例，实际不建议）
    student1.checkTeacherGrades(mathTeacher);
    
    std::cout << std::endl;
    
    // ========== 4. 友元模板示例 ==========
    std::cout << "4. 友元模板示例" << std::endl;
    
    try {
        Matrix<int> m1(2, 3);
        Matrix<int> m2(3, 2);
        
        // 初始化矩阵
        m1.setValue(0, 0, 1); m1.setValue(0, 1, 2); m1.setValue(0, 2, 3);
        m1.setValue(1, 0, 4); m1.setValue(1, 1, 5); m1.setValue(1, 2, 6);
        
        m2.setValue(0, 0, 7); m2.setValue(0, 1, 8);
        m2.setValue(1, 0, 9); m2.setValue(1, 1, 10);
        m2.setValue(2, 0, 11); m2.setValue(2, 1, 12);
        
        std::cout << "矩阵 M1:" << std::endl;
        m1.display();
        
        std::cout << "矩阵 M2:" << std::endl;
        m2.display();
        
        // 使用友元函数进行矩阵乘法
        Matrix<int> result = multiply(m1, m2);
        
        std::cout << "M1 × M2 结果:" << std::endl;
        result.display();
        
    } catch (const std::exception& e) {
        std::cout << "错误: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
    
    // ========== 5. 友元与继承示例 ==========
    std::cout << "5. 友元与继承示例" << std::endl;
    
    Circle circle(5.0);
    
    std::cout << "圆形面积: " << circle.getArea() << std::endl;
    debugShape(circle);   // 基类的友元函数
    debugCircle(circle);  // 派生类的友元函数
    
    std::cout << "\n=== 友元使用要点总结 ===\n" << std::endl;
    
    std::cout << "🎯 友元的主要用途:" << std::endl;
    std::cout << "  1. 操作符重载（如 <<, +, ==）" << std::endl;
    std::cout << "  2. 需要访问多个对象私有数据的函数" << std::endl;
    std::cout << "  3. 紧密耦合的类之间的数据访问" << std::endl;
    std::cout << "  4. 提供特殊的访问接口" << std::endl;
    
    std::cout << "\n✅ 友元的优势:" << std::endl;
    std::cout << "  - 突破封装限制，访问私有成员" << std::endl;
    std::cout << "  - 提供更自然的语法（如操作符重载）" << std::endl;
    std::cout << "  - 实现高效的算法（直接访问内部数据）" << std::endl;
    
    std::cout << "\n⚠️ 使用注意事项:" << std::endl;
    std::cout << "  - 破坏封装性，谨慎使用" << std::endl;
    std::cout << "  - 友元关系不对称、不传递、不继承" << std::endl;
    std::cout << "  - 增加类之间的耦合度" << std::endl;
    std::cout << "  - 应该用于逻辑上紧密相关的类/函数" << std::endl;
    
    std::cout << "\n🎯 典型使用场景:" << std::endl;
    std::cout << "  - I/O 操作符重载（operator<< >>）" << std::endl;
    std::cout << "  - 数学运算符重载（operator+ - * /）" << std::endl;
    std::cout << "  - 比较操作符重载（operator== != < >）" << std::endl;
    std::cout << "  - 容器类的迭代器实现" << std::endl;
    std::cout << "  - 单元测试类访问私有成员" << std::endl;
    std::cout << "  - 工厂模式中的创建函数" << std::endl;
    
    return 0;
}
