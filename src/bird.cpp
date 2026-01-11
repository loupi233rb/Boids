#include "bird.h"
#include "info.h"
#include <cmath>
#include <random>


vector2 vector2::operator+(const vector2& other) const{
    return vector2(x+other.x,y+other.y);
};

vector2 vector2::operator-(const vector2& other) const{
    return vector2(x-other.x,y-other.y); 
};

vector2 vector2::operator*(float scalar) const{
    return vector2(x*scalar, y*scalar);
};

vector2 vector2::operator/(float num) const{
    return vector2(x/num,y/num);
}

vector2& vector2::operator+=(const vector2& other){
    *this = *this + other;
    return *this;
};
vector2& vector2::operator-=(const vector2& other){
    *this = *this - other;
    return *this;
};
vector2& vector2::operator*=(float s){
    *this = *this * s;
    return *this;
};
vector2& vector2::operator/=(float s){
    *this = *this / s;
    return *this;
};
void vector2::zero(){
    x = 0.;
    y = 0.;
}

// 最大速度改变量限制
vector2 vector2::limit(float maxForce) {
    if(lenth()<=maxForce) return *this;
    else return this->normalize()*maxForce;
}

double vector2::lenth() const{
    return sqrt(x*x+y*y);
};

vector2 vector2::normalize() const{
    float len = lenth();
    if(len>0) return vector2(x/len,y/len);
    return vector2(0,0);
};

double vector2::distanceTo(const vector2 other) const {
    return sqrt(pow(x-other.x,2)+pow(y-other.y,2));
};

void vector2::print(){
    printf("(%f, %f)",x,y);
}

vector2 vector2::rotate(float theta){
    vector2 rv;
    float rx = x*cos(theta)-y*sin(theta);
    float ry = x*sin(theta)+y*cos(theta);
    rv.x = rx;
    rv.y = ry;
    return rv;
}



bird::bird(float s, vector2 initV, vector2 initP){
    scale = s;
    position = initP;
    prevPosition = initP;
    velocity = initV;
    prevVelocity = initV;
    acceleration = vector2(0.,0.);
    isCaptain = 0;
    gen = std::mt19937(std::random_device{}());
}

bird::~bird(){};



vector2 bird::getPos() const{return position;}

vector2 bird::getV() const {return velocity;}

void bird::update(CellGrid &cellgrid){

    std::vector<bird*> sep_bird = cellgrid.getNeibors(this, "separation");
    std::vector<bird*> ali_bird = cellgrid.getNeibors(this, "alignment");
    std::vector<bird*> coh_bird = cellgrid.getNeibors(this, "cohesion");

    vector2 s = Rule::Separation_v2(this, sep_bird, eset, bset) * bset.s_w;
    vector2 a = Rule::Alignment(this, ali_bird, eset, bset) * bset.a_w;
    vector2 c = Rule::Cohesion(this, coh_bird, eset, bset) * bset.c_w;
    vector2 chase = Rule::ChaseMouse(this, eset, bset);
    vector2 avoidance = Rule::AvoidBoundary(this, eset);

    if(avoidance.lenth()!=0){
        s.zero();
        a.zero();
        c.zero();
    }
    if(chase.lenth()!=0){
        a.zero();
        c.zero();
    }

    std::normal_distribution<> ldist(0,0.5);
    std::normal_distribution<> rdist(0,M_PI/24);
    double noise = ldist(gen) * bset.noise_w;
    double force = (s + a + c + chase + avoidance).lenth();
    acceleration = (s + a + c + chase + avoidance).normalize().rotate(rdist(gen)) * force;
};

inline void limitPos(vector2& vec){
    if(vec.x < 0) vec.x = 0;
    if(vec.y < 0) vec.y = 0;
    if(vec.x >= eset.MX) vec.x = eset.MX - 1e-3;
    if(vec.y >= eset.MY) vec.y = eset.MY - 1e-3;
}

void bird::tick_v2_0(){
    if(acceleration.lenth() > bset.f_max){
        acceleration = acceleration.normalize() * bset.f_max;
    }
    if(acceleration.lenth() < bset.f_min){
        acceleration = vector2(0.,0.);
    }
    prevVelocity = velocity;
    velocity += acceleration * eset.DT;
    if(velocity.lenth() > bset.v_max){
        velocity = velocity.normalize() * bset.v_max;
    }
    prevPosition = position;
    position += velocity * eset.DT;
    limitPos(position);
    // 更新颜色
    this->RefreshColor(bset.v_max);
};

void bird::RefreshColor(double max)
{
    int r,g,b;
    double l = this->getV().lenth() / max;
    if(l <= 0.25){
        r = 0;
        g = int(255 * l / 0.25);
        b = 255;
    }
    else if(l <= 0.5){
        r = 0;
        g = 255;
        b = 255 - int(255 * (l-0.25) / 0.25);
    }
    else if(l <= 0.75){
        r = int(255 * (l-0.5) / 0.25);
        g = 255;
        b = 0;
    }
    else{
        r = 255;
        g = 255 - int(255 * (l-0.75) / 0.25);
        b = 0;
    }
    this->color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}

namespace Birdmath
{
    // soft inverse proportion
    double SIP(double a, double x){
        return a/(x + 1);
    };
};

namespace Rule
{
    vector2 Separation(bird* self, const std::vector<bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset){
        vector2 sep_vector(0, 0);
        int sep_count = 0;
        for(int i = 0;i < boids.size();i++){
            if(boids[i] == self) continue;
            if(distance[i] < bset.s_r){
                sep_vector += boids[i]->getPos();
                sep_count++;
            }
        }
        if(sep_count != 0) sep_vector /= sep_count;
        return (self->getPos() - sep_vector).normalize() * Birdmath::SIP(10., (self->getPos() - sep_vector).lenth());
    };

    vector2 Separation_v2(bird* self, const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset){
        vector2 sep_force(0,0);
        for(auto *i: boids){
            if(i == self) continue;
            double distance = self->getPos().distanceTo(i->getPos());
            if(distance < bset.s_r && distance > 1e-3f){
                sep_force += (self->getPos() - i->getPos()) / distance * bset.s_r;
            }
        }
        return sep_force;
    }

    vector2 Cohesion(bird* self, const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset){
        vector2 coh_vector(0, 0);
        int coh_count = 0;
        for(auto *i: boids){
            if(i == self) continue;
            double distance = self->getPos().distanceTo(i->getPos());
            if(distance < bset.c_r && distance > 1e-3f){
                coh_vector += i->getPos();
                coh_count++;
            }
        }
        if(coh_count != 0) coh_vector /= coh_count;
        return coh_vector - self->getPos();
    };

    vector2 Alignment(bird* self, const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset){
        vector2 ali_vector(0,0);
        int ali_count = 0;
        for(auto *i: boids){
            if(i == self) continue;
            double distance = self->getPos().distanceTo(i->getPos());
            if(distance < bset.a_r){
                ali_vector += i->getV();
                ali_count++;
            }
        }
        if(ali_count != 0) ali_vector /= ali_count;
        return ali_vector - self->getV() * bset.a_w;
    };

    // 鼠标位置换为glm::vec2
    vector2 ChaseMouse(bird* self, const EnvSetting &eset, const BirdSetting &bset){
        if(eset.IS_LEFTBUTTON_DOWN){
            double d = self->getPos().distanceTo(vector2(eset.MOUSE_POSITION.x, eset.MOUSE_POSITION.y));
            if(d > 1.5*bset.mouse_r){
                return vector2(eset.MOUSE_POSITION.x, eset.MOUSE_POSITION.y) - self->getPos();
            }
            else if(d <= 1.5 * bset.mouse_r && d >= 0.5 * bset.mouse_r){
                vector2 chase = vector2(eset.MOUSE_POSITION.x, eset.MOUSE_POSITION.y) - self->getPos();
                chase.rotate(M_PI_2);
                return chase;
            }
            else{
                return (self->getPos() - vector2(eset.MOUSE_POSITION.x, eset.MOUSE_POSITION.y)).normalize() * Birdmath::SIP(bset.mouse_r, d);
            }
        }
        else{
            return vector2(0.,0.);
        }
    };

    vector2 AvoidBoundary(bird* self, const EnvSetting &eset){
        vector2 boundary_force;
        if(self->getPos().x < eset.BOUNDARY) boundary_force.x += (eset.BOUNDARY - self->getPos().x)/eset.BOUNDARY;
        if(self->getPos().x > eset.MX-eset.BOUNDARY) boundary_force.x -= (self->getPos().x-(eset.MX-eset.BOUNDARY))/eset.BOUNDARY;
        if(self->getPos().y < eset.BOUNDARY) boundary_force.y += (eset.BOUNDARY - self->getPos().y)/eset.BOUNDARY;
        if(self->getPos().y > eset.MY-eset.BOUNDARY) boundary_force.y -= (self->getPos().y - (eset.MY-eset.BOUNDARY))/eset.BOUNDARY;
        return boundary_force.normalize() * eset.BOUNDARY_FORCE;
    }
}

void CellGrid::Initialize(){
    sepnet.rows = std::ceil(eset.MX / bset.s_r);
    sepnet.cols = std::ceil(eset.MY / bset.s_r);
    sepnet.net.resize(sepnet.rows * sepnet.cols);

    alinet.rows = std::ceil(eset.MX / bset.a_r);
    alinet.cols = std::ceil(eset.MY / bset.a_r);
    alinet.net.resize(alinet.rows * alinet.cols);

    cohnet.rows = std::ceil(eset.MX / bset.c_r);
    cohnet.cols = std::ceil(eset.MY / bset.c_r);
    cohnet.net.resize(cohnet.rows * cohnet.cols);
}

void CellGrid::refresh(){
    for(auto &cell : sepnet.net){
        cell.clear();
    }
    for(auto &cell : alinet.net){
        cell.clear();
    }
    for(auto &cell : cohnet.net){
        cell.clear();
    }
    
    for(auto *b : birds){
        int r, c, idx;
        // separation
        r = std::floor(b->getPos().x / bset.s_r);
        c = std::floor(b->getPos().y / bset.s_r);
        idx = r * sepnet.cols + c;
        sepnet.net[idx].push_back(b);
        // alignment
        r = std::floor(b->getPos().x / bset.a_r);
        c = std::floor(b->getPos().y / bset.a_r);
        idx = r * alinet.cols + c;
        alinet.net[idx].push_back(b);
        // cohesion
        r = std::floor(b->getPos().x / bset.c_r);
        c = std::floor(b->getPos().y / bset.c_r);
        idx = r * cohnet.cols + c;
        cohnet.net[idx].push_back(b);
    }
}

std::vector<bird*> CellGrid::getNeibors(bird* self, const std::string &type){
    std::vector<bird*> neighbors;
    int r, c, idx;
    cellnet targetnet;
    int grid_size;
    if(type == "separation"){
        targetnet = sepnet;
        grid_size = bset.s_r;
    }
    else if(type == "alignment"){
        targetnet = alinet;
        grid_size = bset.a_r;
    }
    else if(type == "cohesion"){
        targetnet = cohnet;
        grid_size = bset.c_r;
    }
    else{
        return neighbors;
    }

    r = std::floor(self->getPos().x / grid_size);
    c = std::floor(self->getPos().y / grid_size);

    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            int nr = r + i;
            int nc = c + j;
            if(nr >= 0 && nr < targetnet.rows && nc >= 0 && nc < targetnet.cols){
                idx = nr * targetnet.cols + nc;
                neighbors.insert(neighbors.end(), targetnet.net[idx].begin(), targetnet.net[idx].end());
            }
        }
    }
    return neighbors;
}

