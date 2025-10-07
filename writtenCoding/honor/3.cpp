// 复数运算
// 编写一个函数完成复数的常用二元算数运算操作（加、减、乘、除）。复数的实部和虚部均为整数。复数的表示形式是一对带括号的整数对，
// 例如：(1, 2) 表示实部为1，虚部为2的复数。
// 假设（a, b）和（c, d）分别表示两个复数，则它们的加、减、乘、除运算定义如下：
// 加法：(a, b) + (c, d) = (a + c, b + d)
// 减法：(a, b) - (c, d) = (a - c, b - d)
// 乘法：(a, b) * (c, d) = (a * c - b * d, a * d + b * c)
// 除法：(a, b) / (c, d) = ((a * c + b * d) / (c * c + d * d), (b * c - a * d) / (c * c + d * d))
// 输入描述:
// 输入包括一行，表示两个复数和一个运算符，复数的格式如上所述，运算符为+、-、*、/中的一个。
// 例如：(1000, 1000) - (500, 2000)
// 输出描述:
// 输出一行，表示运算结果复数，格式与输入相同。输入异常返回字符串“NaN”
#include <iostream>
#include <string>
using namespace std;

struct Complex {
	int real, imag;
	Complex(int r = 0, int i = 0) : real(r), imag(i) {}
};

Complex add(const Complex& a, const Complex& b) {
	return Complex(a.real + b.real, a.imag + b.imag);
}
Complex sub(const Complex& a, const Complex& b) {
	return Complex(a.real - b.real, a.imag - b.imag);
}
Complex mul(const Complex& a, const Complex& b) {
	return Complex(a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real);
}
bool div(const Complex& a, const Complex& b, Complex& res) {
	int den = b.real * b.real + b.imag * b.imag;
	if (den == 0) return false;
	res = Complex((a.real * b.real + a.imag * b.imag) / den, (a.imag * b.real - a.real * b.imag) / den);
	return true;
}

int main() {
	string s;
	getline(cin, s);
	int a, b, c, d;
	char op;
	sscanf(s.c_str(), "(%d, %d) %c (%d, %d)", &a, &b, &op, &c, &d);
	Complex x(a, b), y(c, d), res;
	bool ok = true;
	if (op == '+') res = add(x, y);
	else if (op == '-') res = sub(x, y);
	else if (op == '*') res = mul(x, y);
	else if (op == '/') ok = div(x, y, res);
	if (!ok) cout << "NaN" << endl;
	else cout << "(" << res.real << ", " << res.imag << ")" << endl;
	return 0;
}
// 例如：(500, -1000)