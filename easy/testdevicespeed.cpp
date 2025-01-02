#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

// O(n)
void func_1(long long n) {
    long long k = 0;
    for (long long i = 0; i < n; i++) {
        k++;
    }
}

// O(n^2)
void func_2(long long n) {
    long long k = 0;
    for (long long i = 0; i < n; i++) {
        for (long long j = 0; j < n; j++) {
            k++;
        }
    }
}

// O(nlogn)
void func_3(long long n) {
    long long k = 0;
    for (long long i = 0; i < n; i++) {
        for (long long j = 1; j < n; j = j * 2){
            k++;
        }
    }
}

int main(int argc, char *argv[])
{
    long long n;
    while (1) {
        cout << "输入 n: ";
        cin >> n;
        milliseconds start_time = duration_cast<milliseconds> (
            system_clock::now().time_since_epoch()
        );
        // func_1(n);
        func_2(n);
        // func_3(n);
        milliseconds end_time = duration_cast<milliseconds> (
            system_clock::now().time_since_epoch()
        );
        cout << "耗时:" << milliseconds(end_time).count() - milliseconds(start_time).count() << " ms" << endl;

    }
    return 0;
}
