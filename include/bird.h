#pragma once

#define _USE_MATH_DEFINES

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <random>


class bird;
class CellGrid;
struct EnvSetting;
struct BirdSetting;


// 自己写的2D向量结构体，懒得改用glm了
struct vector2{
    double x,y;
    vector2(float x=0,float y=0):x(x),y(y){};
    vector2 operator+(const vector2& other) const;
    vector2 operator-(const vector2& other) const;
    vector2 operator*(float scalar) const;
    vector2 operator/(float num) const;
    vector2& operator+=(const vector2& other);
    vector2& operator-=(const vector2& other);
    vector2& operator*=(float s);
    vector2& operator/=(float s);
    vector2 limit(float maxForce);
    double distanceTo(const vector2 other) const;
    double lenth() const;
    vector2 rotate(float theta);
    vector2 normalize() const;
    void print();
    void zero();
};



class bird{
private:
    float scale;  //大小
    vector2 position;
    vector2 velocity;
    vector2 acceleration;
    bool isCaptain;
    std::mt19937 gen;
public:
    glm::vec3 color;
    vector2 prevPosition;
    vector2 prevVelocity;
    bird(float s, vector2 initV, vector2 initP);
    ~bird();
    vector2 getPos() const;
    vector2 getV() const;
    void update(CellGrid &cellgrid);
    void tick_v2_0();
    void RefreshColor(double max);
};

struct birdInstance{
    glm::vec4 color;
    glm::mat4 model;
};

namespace Rule
{
    vector2 Separation(bird* self, const std::vector<bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset);
    vector2 Cohesion(bird* self, const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset);
    vector2 Alignment(bird* self, const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset);
    vector2 ChaseMouse(bird* self);
    vector2 AvoidBoundary(bird* self, const EnvSetting &eset);
    vector2 Separation_v2(bird* self, const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset);
}

struct cellnet{
    int rows;
    int cols;
    std::vector<std::vector<bird*>> net;
};

class CellGrid{
private:
    cellnet sepnet;
    cellnet alinet;
    cellnet cohnet;
public:
    void Initialize();
    void refresh();
    std::vector<bird*> getNeibors(bird* self, const std::string &type);
};

// class bird_crossList:public CrossList<grid>{
// public:
//     int grid_size;
//     bird_crossList(int rows, int cols):CrossList<grid>(rows, cols){};
//     ~bird_crossList();
//     void add(bird* b);
//     void clear_bird();
// };

// // 网格划分函数
// std::vector<bird_crossList> DivideGrid(const EnvSetting& eset, const BirdSetting& bset);

// // 网格更新
// void GridUpdate(std::vector<bird_crossList> &gridSet, const std::vector<bird*> &boids, const BirdSetting &bset);

// // 返回网格范围
// std::vector<bird*> FindNeighborGridBird(const bird_crossList &grids, bird* b);

// //回收内存
// void DeleteGrids(std::vector<bird_crossList> &gridSet);
