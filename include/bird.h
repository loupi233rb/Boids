#ifndef BIRD_H
#define BIRD_H

#define _USE_MATH_DEFINES

#include <easyx.h>
#include <vector>
#include <cmath>
#include <random>

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
    float distanceTo(const vector2 other);
    float lenth() const;
    vector2 rotate(float theta);
    vector2 normalize() const;
    POINT to_POINT();
    void print();
    void zero();
};


// 环境设置
struct EnvSetting{
    int MX = 800;        // window width
    int MY = 600;        // window height
    int BOUNDARY = 15;
    int BOUNDARY_FORCE = 100;
    double DT = 0.02;
    int LOGIC_FPS = 30;
    int RENDER_FPS = 60;
    int BIRD_NUM = 50;
    bool IS_LEFTBUTTON_DOWN = 0;
    vector2 MOUSE_POSITION = vector2(0,0);
    EnvSetting() = default;
};

// 鸟设置
struct BirdSetting{
    float s_w = 1.2;              // separation weight
    float a_w = 2.0;              // alignment weight
    float c_w = 1.0;              // cohesion weight
    int s_r = 50;                 // separation range
    int a_r = 200;                // alignment range
    int c_r = 50;                 // cohesion range
    double v_max = 100;           // max velocity
    double f_max = 100;           // max force
    double f_min = 15;            // min force
    double mouse_r = 200;         // mouse react range
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
    void tick(float dt, float maxSpeed, int mx, int my, int random_step);
    vector2 getPos();
    vector2 getV();
    void think(std::vector<bird*> &boids, int sep_range, int par_range, int agg_range, float s_weight, float p_weight, float a_weight, float maxForce, float minForce, bool is_pressed, vector2 mouse_position, int mouse_range, int chase_speed);
    void update(const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset);
    void tick_v2_0(const EnvSetting &eset, const BirdSetting &bset);
    void render(const std::vector<vector2> &shape, int point_num);

};

namespace Rule
{
    vector2 Separation(bird* self, const std::vector<bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset);
    vector2 Cohesion(bird* self, const std::vector<bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset);
    vector2 Alignment(bird* self, const std::vector<bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset);
    vector2 ChaseMouse(bird* self, const EnvSetting &eset, const BirdSetting &bset);
    vector2 AvoidBoundary(bird* self, const EnvSetting &eset);
}

#endif