//
// Created by Marco Siracusa on 5/29/18.
//

#ifndef AAPROJECT_DOUBLESTACK_HPP
#define AAPROJECT_DOUBLESTACK_HPP

class DoubleStack {

private:
    int *items;
    int capacity;
    int fp; // front pointer
    int bp; // back pointer

public:
    DoubleStack(int capacity) {
        this->items = (int *) malloc(sizeof(int) * capacity);
        this->capacity = capacity;
        this->fp = 0;
        this->bp = capacity;

    }

    ~DoubleStack() {
        free(this->items);
    }

    // ============================
    // Front stack
    // ============================

    bool isEmptyFront() {
        return fp == 0;
    }

    int topFront() {
        return items[fp - 1];
    }

    int popFront() {
        return items[--fp];
    }

    void pushFront(int item) {
        items[fp++] = item;
    }

    // ============================
    // Back stack
    // ============================

    bool isEmptyBack() {
        return bp == capacity;
    }

    int topBack() {
        return items[bp];
    }

    int popBack() {
        return items[bp++];
    }

    void pushBack(int item) {
        items[--bp] = item;
    }
};

#endif //AAPROJECT_DOUBLESTACK_HPP
