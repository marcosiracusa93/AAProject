//
// Created by Marco Siracusa on 5/29/18.
//

#ifndef AAPROJECT_DOUBLESTACKVEC_HPP
#define AAPROJECT_DOUBLESTACKVEC_HPP

#include "typedefs.hpp"

template<class T>
class DoubleStackVec {

private:
    std::vector<T> frontStack;
    std::vector<T> backStack;

public:
    DoubleStackVec(int capacity) : frontStack(std::vector<T>()), backStack(std::vector<T>()) {}

    // ============================
    // Front stack
    // ============================

    bool isEmptyFront() {
        return frontStack.size() == 0;
    }

    T topFront() {
        return frontStack.at(frontStack.size() - 1);
    }

    T popFront() {
        T ret = topFront();
        frontStack.pop_back();
        return ret;
    }

    void pushFront(T item) {
        frontStack.push_back(item);
    }

    // ============================
    // Back stack
    // ============================

    bool isEmptyBack() {
        return backStack.size() == 0;
    }

    T topBack() {
        return backStack.at(backStack.size() - 1);
    }

    T popBack() {
        T ret = topBack();
        backStack.pop_back();
        return ret;
    }

    void pushBack(T item) {
        backStack.push_back(item);
    }
};

#endif //AAPROJECT_DOUBLESTACKVEC_HPP
