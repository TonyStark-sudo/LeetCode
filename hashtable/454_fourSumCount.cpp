#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
    public:
    int fourSumCount(vector<int>& A, vector<int>& B, vector<int>& C, vector<int>& D) {
        // key:a + b Value:a + b出现的次数
        unordered_map<int, int> sumAB;
        for (int a : A) {
            for (int b : B) {
                sumAB[a + b]++;
            }
        }
        int count = 0;
        for (int c : C) {
            for (int d : D) {
                if (sumAB.find(0 - (c + d)) != sumAB.end()) {
                    count += sumAB[0 - (c + d)];
                }
            }
        }

        return count;
    }
};