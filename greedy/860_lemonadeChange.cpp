#include <vector>

using namespace std;
class Solution {
public:
    bool lemonadeChange(vector<int>& bills) {
        if (bills[0] != 5) return false;
        int five_count = 0, ten_count = 0;
        for (int i = 0; i < bills.size(); i++) {
            if (bills[i] == 5) {
                five_count++;
            }
            else if (bills[i] == 10) {
                five_count--;
                ten_count++;
            }
            else if (bills[i] == 20) {
                if (five_count >= 1 && ten_count >= 1) {
                    five_count--;
                    ten_count--;
                    continue;
                }
                else if(five_count >= 3) {
                    five_count = five_count - 3;
                    continue;
                }
                return false; 
            }
        }
        return true;
    }
};