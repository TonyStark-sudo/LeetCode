#include <vector>
#include <iostream>

using namespace std;

class Solution {
public:
    int _canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
        for (int i = 0; i < cost.size(); i++) {
            int remain_gas = gas[i] - cost[i];
            int index = (i + 1) % cost.size();
            while (remain_gas > 0 && index != i) {
                remain_gas += gas[index] - cost[index];
                index = (++index) % cost.size();
            }
            if (remain_gas >= 0 && index == i) return i;
        }
        return -1;
    }

    int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
        int remain_gas = 0, total_sum = 0, start_index = 0;
        for (int i = 0; i < gas.size(); i++) {
            remain_gas += gas[i] - cost[i];
            total_sum += gas[i] - cost[i];
            if (remain_gas < 0) {
                start_index = i + 1;
                remain_gas = 0;
            }
        }
        return total_sum < 0 ? -1 : start_index;
    }
};