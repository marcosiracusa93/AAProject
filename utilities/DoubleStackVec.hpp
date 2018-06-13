//
// Created by Marco Siracusa on 5/29/18.
//

#ifndef AAPROJECT_DOUBLESTACKVEC_HPP
#define AAPROJECT_DOUBLESTACKVEC_HPP

class DoubleStackVec {

private:
    std::vector<int> frontStack;
    std::vector<int> backStack;

public:
    DoubleStackVec(int capacity) : frontStack(std::vector<int>()), backStack(std::vector<int>()) {}

    // ============================
    // Front stack
    // ============================

    bool isEmptyFront() {
        return frontStack.size() == 0;
    }

    int topFront() {
        return frontStack.at(frontStack.size() - 1);
    }

    int popFront() {
        int ret = topFront();
        frontStack.pop_back();
        return ret;
    }

    void pushFront(int item) {
        frontStack.push_back(item);
    }

    // ============================
    // Back stack
    // ============================

    bool isEmptyBack() {
        return backStack.size() == 0;
    }

    int topBack() {
        return backStack.at(backStack.size() - 1);
    }

    int popBack() {
        int ret = topBack();
        backStack.pop_back();
        return ret;
    }

    void pushBack(int item) {
        backStack.push_back(item);
    }
};

#endif //AAPROJECT_DOUBLESTACKVEC_HPP
