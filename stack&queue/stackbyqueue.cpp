#include <queue>
#include <iostream>

class MyStack {
public:
    MyStack() {

    }

    void push(int x) {
        queIn.push(x);
    }

    int pop() {
        int size = queIn.size();
        if (queOut.empty()) {
            while (size--) {
                queOut.push(queIn.back());
            }
            while (!queIn.empty()) {
                queIn.pop();
            }
        }

        int res = queOut.front();
        queOut.pop();
        return res; 
    }
                         
     int top() {
        int res = this->pop();
        queIn.push(res);
        return res;
    }

    bool empty() {
        return queIn.empty() && queOut.empty();
    }
 
private:
    std::queue<int> queIn;
    std::queue<int> queOut;

};

int main(int argc, char *argv[])
{
    std::queue<int> q;
    std::vector<int> nums(10, 0);

    for (int i = 0; i < nums.size(); i++) {
        nums[i] = i;
        // nums.push_back(i);
    }

    for (int num : nums) {
        q.push(num);
    }

    int pop, top, size, back;
    std::cout << "front: " << q.front() << std::endl;
    std::cout << "back: " << q.back() << std::endl;
    /* code */
    return 0;
}
