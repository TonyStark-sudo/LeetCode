#include "project.h"
#include <iostream>

int main(int argc, char **argv) {
    std::cout << "main" << std::endl;
    if (argc < 2) {
        // std::cout << "argc < 2" << std::endl;
        return -1;
    }
    Projector projector(argv[1]);
    projector.Project();
    return 0;
}
