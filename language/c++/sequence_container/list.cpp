#include <list>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    list<pair<int, string>> name_list;
    name_list = {{1, "lili"}, {2, "Diana"}, {3, "Kiki"}};
    
    for (auto& person : name_list) {
        std::cout << person.first << "----" << person.second << std::endl;
    }
    // name_list.remove({2, "Diana"});
    list<pair<int, string>>::iterator it = name_list.begin();
    std::advance(it, 2);
    for (auto& person : name_list) {
        std::cout << person.first << "----" << person.second << std::endl;
    }
    // name_list.reverse();
    name_list.insert(name_list.begin(), {0, "vicky"});
    for (auto& person : name_list) {
        std::cout << person.first << "----" << person.second << std::endl;
    }
    return 0;
}
