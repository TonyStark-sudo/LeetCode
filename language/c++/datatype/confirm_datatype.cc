#include <iostream>
#include <cmath>

using namespace std;

int main(int argc, char const *argv[])
{
    cout << "double type size: " << sizeof(double) << endl;
    cout << "long double type size: " << sizeof(long double) << endl;
    cout << "double_t type size: " << sizeof(double_t) << endl;
    cout << "float type size: " << sizeof(float) << endl;
    cout << "int type size: " << sizeof(int) << endl;
    if (0.1 + 0.2 == 0.3) {
        cout << "0.1 + 0.2 = 0.3" << endl;
    }
    else {
        cout << "0.1 + 0.2 != 0.3" << endl;
    }
    return 0;
}

// #include <cmath>

// int main(int argc, char const *argv[])
// {
//     // ...existing code...
//     double a = 0.1, b = 0.2, c = 0.3;
//     double sum = a + b;
//     cout.setf(ios::fixed); cout.precision(17);
//     cout << "a=" << a << ", b=" << b << ", sum=" << sum << ", c=" << c << endl;

//     const double eps = 1e-12;
//     if (fabs(sum - c) < eps) {
//         cout << "approximately equal (within eps)" << endl;
//     } else {
//         cout << "not equal" << endl;
//     }
//     // ...existing code...
//     return 0;
// }