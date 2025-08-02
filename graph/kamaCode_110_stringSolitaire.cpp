#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

int stringListSize;
int main(int argc, char const *argv[])
{
    cin >> stringListSize;
    string beginStr, endStr;
    cin >> beginStr >> endStr;
    vector<string> stringList(stringListSize, string(""));
    for (int i = 0; i < stringListSize; i++) {
        cin >> stringList[i];
    }

    queue<string> wordsQueue;
    unordered_map<string, int> wordsMap;
    unordered_set<string> wordsSet(stringList.begin(), stringList.end());
    wordsQueue.push(beginStr);
    // wordsMap.emplace(beginStr, 1);
    wordsMap[beginStr] = 1;

    while (!wordsQueue.empty()) {
        string word = wordsQueue.front();
        wordsQueue.pop();
        for (size_t i{0UL}; i < word.size(); ++i) {
            string new_word = word;
            for (size_t j{0UL}; j < 26; j++) {
                new_word[i] = 'a' + j;
                if (new_word == endStr) {
                    cout << wordsMap[word] + 1 << endl;
                    return 0;
                }
                // 通过wordsMap来判断新的单词是否遇到过，并且记录到该单词需要用到的次数
                if (wordsSet.find(new_word) != wordsSet.end() && wordsMap.find(new_word) == wordsMap.end()) {
                    wordsQueue.push(new_word);
                    wordsMap[new_word] = wordsMap[word] + 1;
                }
            }
        }
    }
    cout << 0 << endl;
    // for (int i = 0; i < stringListSize; i++) {
    //     cout << stringList[i] << " ";
    // }
    // cout << endl;
    
    return 0;
}
