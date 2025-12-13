#ifndef _CROSS_LIST_H
#define _CROSS_LIST_H

#include <iostream>
#include <vector>

template<typename T>
struct CrossNode{
    int row;
    int col;
    T* data;
    CrossNode<T> *left, *right, *up, *down;
    CrossNode();
};

template<typename T>
class CrossList{
protected:
    CrossNode<T> **row, **col;
    int rows;
    int cols;
    int data_num;
public:
    CrossList();
    CrossList(int rows, int cols);
    ~CrossList();
    T* get(int r, int c);
    void clear();
};

#include "crossList.tpp"

#endif