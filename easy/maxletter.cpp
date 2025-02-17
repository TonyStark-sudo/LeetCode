#include <iostream>
#include <vector>

using namespace std;

void solution(const std::vector<char>& a) {
    int hash_array[26] = {0};
    int max_count = 0;
    char result;
    for (int i = 0; i < a.size(); i++) {
        hash_array[a[i] - 'a'] ++;
    }

    for (int i = 0; i < 26; i++) {
        if (hash_array[i] > max_count) {
            max_count = hash_array[i];
            result = i + 'a';
        }
    }

    cout << "出现最多的字母是： " << result << ", 出现了" << max_count << "次" << endl;
}

