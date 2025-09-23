#include <optional>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

// ========== 基础用法示例 ==========
std::optional<int> parseInt(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return std::nullopt;  // 没有值
    }
}

// ========== 查找函数示例 ==========
std::optional<std::string> findUser(int id) {
    static std::vector<std::pair<int, std::string>> users = {
        {1, "Alice"}, {2, "Bob"}, {3, "Charlie"}
    };
    
    auto it = std::find_if(users.begin(), users.end(),
        [id](const auto& user) { return user.first == id; });
    
    if (it != users.end()) {
        return it->second;
    }
    return std::nullopt;  // 未找到用户
}

// ========== 除法运算示例 ==========
std::optional<double> safeDivide(double a, double b) {
    if (b != 0.0) {
        return a / b;
    }
    return std::nullopt;  // 除零错误
}

// ========== 配置解析示例 ==========
struct Config {
    std::optional<int> port;
    std::optional<std::string> host;
    std::optional<bool> debug;
};

Config parseConfig(const std::vector<std::string>& args) {
    Config config;
    
    for (const auto& arg : args) {
        if (arg.find("--port=") == 0) {
            auto portStr = arg.substr(7);
            config.port = parseInt(portStr);
        } else if (arg.find("--host=") == 0) {
            config.host = arg.substr(7);
        } else if (arg == "--debug") {
            config.debug = true;
        }
    }
    
    return config;
}

// ========== 链式操作示例 ==========
class Calculator {
public:
    std::optional<double> calculate(const std::string& expression) {
        // 简化示例：只处理 "a+b" 格式
        auto plusPos = expression.find('+');
        if (plusPos == std::string::npos) {
            return std::nullopt;
        }
        
        auto leftStr = expression.substr(0, plusPos);
        auto rightStr = expression.substr(plusPos + 1);
        
        try {
            double left = std::stod(leftStr);
            double right = std::stod(rightStr);
            return left + right;
        } catch (...) {
            return std::nullopt;
        }
    }
};

int main() {
    std::cout << "=== C++17 std::optional 用法示例 ===\n" << std::endl;
    
    // ========== 1. 基础用法 ==========
    std::cout << "1. 基础用法 - 字符串转整数" << std::endl;
    
    auto val1 = parseInt("42");
    auto val2 = parseInt("abc");
    
    if (val1) {                    // 检查是否有值
        std::cout << "解析成功: " << *val1 << std::endl;  // 用*取出值
    } else {
        std::cout << "解析失败" << std::endl;
    }
    
    if (val2.has_value()) {        // 另一种检查方式
        std::cout << "解析成功: " << val2.value() << std::endl;  // 用value()取出值
    } else {
        std::cout << "解析 'abc' 失败" << std::endl;
    }
    
    std::cout << std::endl;
    
    // ========== 2. 创建optional的方式 ==========
    std::cout << "2. 创建optional的不同方式" << std::endl;
    
    std::optional<int> opt1;                    // 空的optional
    std::optional<int> opt2 = std::nullopt;     // 显式设为空
    std::optional<int> opt3 = 42;               // 直接赋值
    std::optional<int> opt4{100};               // 初始化列表
    auto opt5 = std::make_optional<int>(200);   // 使用make_optional
    
    std::cout << "opt1 有值: " << opt1.has_value() << std::endl;
    std::cout << "opt3 的值: " << opt3.value() << std::endl;
    std::cout << "opt5 的值: " << *opt5 << std::endl;
    
    std::cout << std::endl;
    
    // ========== 3. 查找操作 ==========
    std::cout << "3. 查找操作示例" << std::endl;
    
    auto user1 = findUser(2);
    auto user2 = findUser(999);
    
    if (user1) {
        std::cout << "找到用户: " << *user1 << std::endl;
    }
    
    if (!user2) {
        std::cout << "用户ID 999 不存在" << std::endl;
    }
    
    std::cout << std::endl;
    
    // ========== 4. 安全的数学操作 ==========
    std::cout << "4. 安全的除法操作" << std::endl;
    
    auto result1 = safeDivide(10.0, 2.0);
    auto result2 = safeDivide(10.0, 0.0);
    
    std::cout << "10 / 2 = " << result1.value_or(0.0) << std::endl;  // value_or提供默认值
    std::cout << "10 / 0 = " << result2.value_or(-1.0) << " (使用默认值)" << std::endl;
    
    std::cout << std::endl;
    
    // ========== 5. 配置解析 ==========
    std::cout << "5. 配置解析示例" << std::endl;
    
    std::vector<std::string> args = {"--port=8080", "--host=localhost", "--debug"};
    Config config = parseConfig(args);
    
    std::cout << "端口: " << config.port.value_or(3000) << std::endl;
    std::cout << "主机: " << config.host.value_or("0.0.0.0") << std::endl;
    std::cout << "调试模式: " << (config.debug.value_or(false) ? "开启" : "关闭") << std::endl;
    
    std::cout << std::endl;
    
    // ========== 6. 异常安全的值获取 ==========
    std::cout << "6. 异常安全的值获取" << std::endl;
    
    std::optional<int> emptyOpt;
    
    try {
        // value() 在没有值时会抛出异常
        int val = emptyOpt.value();
        std::cout << "值: " << val << std::endl;
    } catch (const std::bad_optional_access& e) {
        std::cout << "访问空optional异常: " << e.what() << std::endl;
    }
    
    // 安全的替代方案
    int safeVal = emptyOpt.value_or(999);
    std::cout << "安全获取值: " << safeVal << std::endl;
    
    std::cout << std::endl;
    
    // ========== 7. 链式操作和函数组合 ==========
    std::cout << "7. 链式操作示例" << std::endl;
    
    Calculator calc;
    
    auto processExpression = [&calc](const std::string& expr) {
        auto result = calc.calculate(expr);
        if (result) {
            std::cout << expr << " = " << *result << std::endl;
        } else {
            std::cout << "无法计算表达式: " << expr << std::endl;
        }
    };
    
    processExpression("3.5+2.1");
    processExpression("invalid");
    
    std::cout << std::endl;
    
    // ========== 8. optional的比较操作 ==========
    std::cout << "8. optional的比较操作" << std::endl;
    
    std::optional<int> a = 10;
    std::optional<int> b = 20;
    std::optional<int> c;  // 空
    std::optional<int> d = 10;
    
    std::cout << "a == d: " << (a == d) << std::endl;  // true
    std::cout << "a < b: " << (a < b) << std::endl;    // true
    std::cout << "a == c: " << (a == c) << std::endl;  // false (有值 vs 无值)
    std::cout << "c == std::nullopt: " << (c == std::nullopt) << std::endl;  // true
    
    std::cout << std::endl;
    
    // ========== 9. optional与容器 ==========
    std::cout << "9. optional与容器" << std::endl;
    
    std::vector<std::optional<int>> optVec = {1, std::nullopt, 3, std::nullopt, 5};
    
    std::cout << "容器中的有效值: ";
    for (const auto& opt : optVec) {
        if (opt) {
            std::cout << *opt << " ";
        }
    }
    std::cout << std::endl;
    
    // 统计有效值数量
    int validCount = std::count_if(optVec.begin(), optVec.end(),
        [](const auto& opt) { return opt.has_value(); });
    std::cout << "有效值数量: " << validCount << std::endl;
    
    std::cout << std::endl;
    
    // ========== 10. reset() 和 emplace() 操作 ==========
    std::cout << "10. reset() 和 emplace() 操作" << std::endl;
    
    std::optional<std::string> strOpt = "Hello";
    std::cout << "初始值: " << *strOpt << std::endl;
    
    strOpt.reset();  // 清空optional
    std::cout << "reset后有值: " << strOpt.has_value() << std::endl;
    
    strOpt.emplace("World");  // 就地构造新值
    std::cout << "emplace后的值: " << *strOpt << std::endl;
    
    // 用emplace构造复杂对象
    std::optional<std::string> complexOpt;
    complexOpt.emplace(10, 'A');  // 构造包含10个'A'的字符串
    std::cout << "复杂构造: " << *complexOpt << std::endl;
    
    std::cout << "\n=== std::optional 使用要点总结 ===\n" << std::endl;
    
    std::cout << "✅ 优势:" << std::endl;
    std::cout << "  - 类型安全的空值表示" << std::endl;
    std::cout << "  - 避免使用nullptr或特殊值" << std::endl;
    std::cout << "  - 明确的API语义（可能有值或无值）" << std::endl;
    std::cout << "  - 丰富的操作接口" << std::endl;
    
    std::cout << "\n🎯 适用场景:" << std::endl;
    std::cout << "  - 函数可能失败的返回值" << std::endl;
    std::cout << "  - 可选的配置参数" << std::endl;
    std::cout << "  - 查找操作的结果" << std::endl;
    std::cout << "  - 延迟初始化的成员变量" << std::endl;
    
    std::cout << "\n⚠️ 注意事项:" << std::endl;
    std::cout << "  - 需要C++17支持" << std::endl;
    std::cout << "  - value()在空值时抛出异常" << std::endl;
    std::cout << "  - 优先使用value_or()提供默认值" << std::endl;
    std::cout << "  - 空optional比较时有特殊规则" << std::endl;
    
    return 0;
}
