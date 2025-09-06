#  已知一套试卷包含多个x-dp算法，即x类型的dp（保证x不为空切不含“d”和“p”两个字符）。例如sosdp,adp, 其拼接起来为sosdpadp, 构成了一套完整的试卷。现在便可以得到该试卷中存在若干类型的dp算法，你需要知道多少本质种不同的dp算法，即有多少种不同x类型的算法，保证s可以被唯一的分割为一个或多个形如x+dp的段。输入描述：在一行上输入一个仅由小写字母组成的字符串s（3≤length(s)≤10^6），表示试卷。输出描述：输出一个整数，表示给定试卷中存在多少种不同类型的dp算法。请写python代码，以ACM的形式呈现。

# 解题思路：
# 1. 题目保证字符串可以唯一分割为若干个"x+dp"的形式
# 2. x不含'd'和'p'字符，且不为空
# 3. 从左到右贪心分割，遇到连续的非'd'非'p'字符作为x，然后必须跟"dp"
# 4. 用集合记录所有不同的x类型

def solve():
    s = input().strip()
    n = len(s)
    
    # 存储所有不同的x类型
    x_types = set()
    
    i = 0
    while i < n:
        # 从当前位置开始寻找一个有效的x+dp段
        start = i
        # 先跳过所有不是'd'或'p'的字符作为x
        while i < n and s[i] != 'd' and s[i] != 'p':
            i += 1
        
        # 检查是否找到"dp"
        if i < n - 1 and s[i] == 'd' and s[i + 1] == 'p':
            x = s[start:i]
            if len(x) > 0:  # x不能为空
                x_types.add(x)
                i += 2  # 跳过"dp"
        else:
            # 如果没有找到有效的"dp"，说明格式有问题
            break
    
    return len(x_types)

# 测试案例（注释掉，提交时删除）
# 输入: "sosdpadp" 
# 分割: "sos" + "dp" + "a" + "dp"
# 输出: 2 (sos, a)

print(solve())
