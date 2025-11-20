#ifndef BIRD_H
#define BIRD_H
#endif
#define _USE_MATH_DEFINES

#include <easyx.h>
#include <vector>
#include <cmath>
#include <random>

struct vector2{
    float x,y;
    vector2(float x=0,float y=0):x(x),y(y){};
    vector2 operator+(const vector2& other) const;
    vector2 operator-(const vector2& other) const;
    vector2 operator*(float scalar) const;
    vector2 operator/(float num) const;
    vector2 limit(float maxForce);
    float distanceTo(const vector2 other);
    float lenth() const;
    vector2 rotate(float theta);
    vector2 normalize() const;
    POINT to_POINT();
    void print();
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
    void think(std::vector<bird*> &boids, int sep_range, int par_range, int agg_range, float s, float p, float a, float maxForce, float minForce, bool is_pressed, vector2 mouse_position, int mouse_range, int chase_speed);
    void render(vector2* shape, int point_num);
    bool chase_mouse(bool is_pressed, vector2 mouse_position, int mouse_range, int chase_speed, double dt);
};