#include <vector>
#include <string>

using namespace std;

class Solution {
private:
    string IpAddress;
    vector<string> IpAddresses;
    int segnum = 0;
    bool isValid(const string& sub_address) {
        if (sub_address[0] == '0' && sub_address.length() > 1) return false;
        return stoi(sub_address) >= 0 && stoi(sub_address) <= 255;
    }
    void backtracking(const string& s, int startIndex) {
        if (startIndex >= s.size() && segnum == 4) {
            IpAddress.pop_back();
            IpAddresses.push_back(IpAddress);
            IpAddress += ".";
            return ;
        }
        for (int i = startIndex; i < s.size() && (s.size() - i - 1) >= (4 - segnum) && (s.size() - i - 1) <= 3 * (4 - segnum); i++) {
            int pop_count;
            string segment = s.substr(startIndex, i - startIndex + 1);
            if (isValid(segment)) {
                IpAddress += segment;
                IpAddress += ".";
                pop_count = segment.size() + 1;
                backtracking(s, i + 1);
                while(pop_count--) {
                    IpAddress.pop_back();
                }
                segnum--;
            }
        }
    }
public:
    vector<string> restoreIpAddresses(string s) {
        if (s.length() < 4 || s.length() > 12) return {};
        backtracking(s, 0);
        return IpAddresses;
    }
};

class Solution2 {
private:
    vector<string> IpAddresses;
    string IpAddress;
    int seg_count = 0;
    void backtracking(const string& s, int startIndex, string IpAddress) {
        if (startIndex >= s.size() && seg_count == 4) {
            IpAddress.pop_back();
            IpAddresses.push_back(IpAddress);
            return ;
        }
        for (int i = startIndex; i < s.size() && (s.size() - i) >= 4 - seg_count && (s.size() - 1) <= 3 * (4 - seg_count) ; i++) {
            string seg = s.substr(i, i - startIndex + 1);
            if (isValid(seg)) {
                seg_count++;
                backtracking(s, i + 1, IpAddress + seg + ".");
                seg_count--;
            }
        }
    }
    bool isValid(const string& segment) {
        if (segment[0] == '0' && segment.size() > 1) return false;
        return stoi(segment) >= 0 && stoi(segment) <= 255;
    }
public:
    vector<string> restoreIpAddresses(string s) {
        IpAddress.clear();
        backtracking(s, 0, IpAddress);
        return IpAddresses;
    }
};