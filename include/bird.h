#ifndef BIRD_H
#define BIRD_H

#define _USE_MATH_DEFINES

#include <easyx.h>
#include <vector>
#include <cmath>
#include <random>

template<typename T>
struct CrossNode;

template<typename T>
class CrossList;

class bird;

using grid = std::vector<const bird*>;

struct vector2{
    double x,y;
    vector2(float x=0,float y=0):x(x),y(y){};
    vector2 operator+(const vector2& other) const;
    vector2 operator-(const vector2& other) const;
    vector2 operator*(float scalar) const;
    vector2 operator/(float num) const;
    vector2 operator+=(const vector2& other);
    vector2 operator-=(const vector2& other);
    vector2 operator*=(float s);
    vector2 operator/=(float s);
    vector2 limit(float maxForce);
    float distanceTo(const vector2 other) const;
    float lenth() const;
    vector2 rotate(float theta);
    vector2 normalize() const;
    POINT to_POINT();
    void print();
    void zero();
};


// 环境设置
struct EnvSetting{
    int MX = 1000;        // window width
    int MY = 800;        // window height
    int BOUNDARY = 15;
    int BOUNDARY_FORCE = 140;
    double DT = 0.03;
    int LOGIC_FPS = 5;
    int RENDER_FPS = 60;
    double LOGIC_FPS_UPDATE_SPEED = 0.5;
    double RENDER_FPS_UPDATE_SPEED = 0.2;
    int BIRD_NUM = 800;
    bool IS_LEFTBUTTON_DOWN = 0;
    vector2 MOUSE_POSITION = vector2(0,0);
    EnvSetting() = default;
};

// 鸟设置
struct BirdSetting{
    float s_w = 1.0;              // separation weight
    float a_w = 0.8;              // alignment weight
    float c_w = 0.8;              // cohesion weight
    int s_r = 50;                 // separation range
    int a_r = 200;                // alignment range
    int c_r = 80;                 // cohesion range
    double v_max = 120;           // max velocity
    double f_max = 200;           // max force
    double f_min = 5;             // min force
    double mouse_r = 250;         // mouse react range
    double noise_w = 1.0;         // noise weight
    std::vector<vector2> shape;
    BirdSetting() = default;
};

class bird{
private:
    COLORREF color;
    float scale;  //大小
    vector2 position;
    vector2 velocity;
    vector2 acceleration;
    bool isCaptain;
    std::mt19937 gen;
public:
    bird(COLORREF c, float s, vector2 initV, vector2 initP);
    ~bird();
    vector2 getPos() const;
    vector2 getV() const;
    void update(const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset, const std::vector<CrossList<grid*>*>* gridSet);
    void tick_v2_0(const EnvSetting &eset, const BirdSetting &bset);
    void render(const std::vector<vector2> &shape, int point_num);
    void RefreshColor(double max);
};

namespace Rule
{
    vector2 Separation(bird* self, const std::vector<const bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset);
    vector2 Cohesion(bird* self, const std::vector<const bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset);
    vector2 Alignment(bird* self, const std::vector<const bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset);
    vector2 ChaseMouse(bird* self, const EnvSetting &eset, const BirdSetting &bset);
    vector2 AvoidBoundary(bird* self, const EnvSetting &eset);
    vector2 Separation_v2(bird* self, const std::vector<const bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset);
}


#endif