#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

bool compare(const vector<int>& p1, const vector<int>& p2) {
    if (p1[0] == p2[0]) return p1[1] < p2[1];
    return p1[0] > p2[0];
}

class Solution {
public:
    vector<vector<int>> reconstructQueue(vector<vector<int>>& people) {
        std::sort(people.begin(), people.end(), compare);
        vector<vector<int>> queue;
        for (int i = 0; i < people.size(); i++) {
            queue.insert(queue.begin() + people[i][1], people[i]);
        }
        return queue;
    }
};