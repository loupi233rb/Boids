#ifndef _CROSS_LIST_H
#define _CROSS_LIST_H

#include <iostream>
#include <vector>

class bird;
struct vector2;
struct EnvSetting;
struct BirdSetting;

template<typename T>
struct CrossNode{
    int row;
    int col;
    T data;
    CrossNode<T> *left, *right, *up, *down;
    CrossNode();
};

template<typename T>
class CrossList{
private:
    CrossNode<T> **row, **col;
    int rows;
    int cols;
    int data_num;
public:
    CrossList();
    CrossList(int rows, int cols);
    ~CrossList();
    bool set(const bird *b, int grid_size);
    void clear();
    void removeFromColumn(CrossNode<grid*>* node);
    void removeBirds();
    CrossNode<T>* getValue(int r, int c) const;
};


// 网格划分函数
std::vector<CrossList<grid*>*>* DivideGrid(const EnvSetting& eset, const BirdSetting& bset);

// 网格更新
void GridUpdate(std::vector<CrossList<grid*>*>* gridSet, const std::vector<bird*> &boids, const BirdSetting &bset);

// 返回网格范围
std::vector<const bird*> FindNeighborGridBird(const CrossList<grid*>* grids, const bird* b, const int grid_size);

//回收内存
void DeleteGrids(std::vector<CrossList<grid*>*>* gridSet);

#endif