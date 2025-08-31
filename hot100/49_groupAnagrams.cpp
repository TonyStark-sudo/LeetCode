#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

using namespace std;

class Solution {
public:
    // 方法1：暴力法（原有方法，未完善，建议不用）
    bool isAnagrams(const string& original, const string& target) {
        if (original.size() != target.size()) return false;
        string t = target;
        for (char c : original) {
            auto pos = t.find(c);
            if (pos == string::npos) return false;
            t.erase(pos, 1);
        }
        return t.empty();
    }

    // 方法2：哈希表+排序（推荐）
    vector<vector<string>> groupAnagrams_hashsort(vector<string>& strs) {
        unordered_map<string, vector<string>> mp;
        for (auto& s : strs) {
            string key = s;
            sort(key.begin(), key.end());
            mp[key].push_back(s);
        }
        vector<vector<string>> result;
        for (auto& [k, value] : mp) {
            cout << k << ": ";
            for (auto& str : value) {
                cout << str << " ";
            }
            cout << endl;
        }
        for (auto& p : mp) {
            result.push_back(std::move(p.second));
        }
        return result;
    }
};

class MySolution {
public:
    bool isAnagrams(const string& original, const string& target) {
        if (original.size() != target.size()) return false;
        for (size_t i{0UL}; i < original.length(); ++i) {
            if (target.find(original[i]) == std::string::npos) return false;
        }
        return true;
    }
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        vector<vector<string>> result;
        vector<string> path;
        vector<bool> isSearched(strs.size(), false);
        for (int i = 0; i < strs.size(); i++) {
            if (isSearched[i]) continue;
            path.push_back(strs[i]);
            isSearched[i] = true;
            for (int j = 0; j < strs.size(); j++) {
                if (i == j) continue;
                if (!isSearched[j] && isAnagrams(strs[i], strs[j])) {
                    path.push_back(strs[j]);
                    isSearched[j] = true;
                }
            }
            result.push_back(path);
            path.clear();
        }
        return result;
    }
};

int main(int argc, char const *argv[])
{
    Solution sol;
    vector<string> strs = {"eat", "tea", "tan", "ate", "nat", "bat"};
    auto res = sol.groupAnagrams_hashsort(strs);
    for (const auto& group : res) {
        for (const auto& word : group) {
            cout << word << " ";
        }
        cout << endl;
    }
    return 0;
}
