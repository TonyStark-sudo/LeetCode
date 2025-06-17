#include <iostream>
using namespace std;

int main() {
    
    // include if running on windows
    #ifdef _WIN32
        cout << "Hello from Windows!" << endl;
    
    // include if running on linux
    #elif __linux__
        cout << "Hello from Linux!" << endl;
    
    // include if running on some other system
    #elif __APPLE__
        cout << "Hello from an Apple platform!" << endl;
    #else 
        cout << "Hello from an unkown platform!" <, endl;
    #endif
    std::cout << "Time: " << __TIME__ << std::endl;;   
    return 0;
}