#include <iostream>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <iomanip>

// C++17 兼容的 bit_cast 替代实现（C++20 可用 std::bit_cast）
template <typename To, typename From>
To bit_cast_fallback(const From& src) {
	static_assert(sizeof(To) == sizeof(From), "bit_cast size mismatch");
	To dst;
	std::memcpy(&dst, &src, sizeof(To));
	return dst;
}

static std::string bits32(uint32_t x) {
	std::bitset<32> b(x);
	std::string s = b.to_string();
	std::string g;
	g.reserve(40);
	for (int i = 0; i < 32; ++i) {
		g.push_back(s[i]);
		if (i % 4 == 3 && i != 31) g.push_back(' ');
	}
	return g;
}

int main() {
	float a_f = 12345.0f;
	int a_i = 12345;

	// 十六进制格式输出（浮点使用 std::hexfloat）
	std::cout << "a_f: " << std::hexfloat << a_f << "\n";
	std::cout << "a_i: 0x" << std::hex << a_i << std::dec << "\n";

	// 获取 float 的原始位模式
	uint32_t fu = bit_cast_fallback<uint32_t>(a_f);
	std::cout << "a_f bits: " << bits32(fu) << "\n";
	std::cout << "a_i bits: " << bits32(static_cast<uint32_t>(a_i)) << "\n";

	// 额外：分解 IEEE 754 字段
	unsigned sign = (fu >> 31) & 1u;
	unsigned exponent = (fu >> 23) & 0xFFu;
	unsigned fraction = fu & 0x7FFFFFu;
	std::cout << "a_f sign: " << sign
			  << ", exp: " << exponent
			  << ", frac: 0x" << std::hex << fraction << std::dec << "\n";

	return 0;
}
