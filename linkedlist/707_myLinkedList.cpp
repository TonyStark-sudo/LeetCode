#include <iostream>

struct NodeList {
    int value;
    NodeList* next;
    NodeList(int x) : value(x), next(NULL) {}  
};


class myLinkedList {
public:
    struct NodeList {
    int value;
    NodeList* next;
    NodeList(int x) : value(x), next(NULL) {}  
    };

    myLinkedList() {
        _dummyHead = new NodeList(0);
        _size = 0;
    }

    int get(int index) {
        if (index > _size - 1 || index < 0) {
            return -1;
        }

        NodeList* cur = _dummyHead->next;
        while(index--) {
            cur = cur->next;
        }
        return cur->value;
        
    }

    void addAtHead(int val) {

    }

    void addAtTail(int val) {

    }

    void addAtIndex(int index, int val) {

    }

    void deleteAtIndex(int index) {

    }

private:
    int _size;
    NodeList* _dummyHead;

};