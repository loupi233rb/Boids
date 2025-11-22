#include "bird.h"
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

vector2 vector2::operator+=(const vector2& other){
    *this = *this + other;
    return *this;
};
vector2 vector2::operator-=(const vector2& other){
    *this = *this - other;
    return *this;
};
vector2 vector2::operator*=(float s){
    *this = *this * s;
    return *this;
};
vector2 vector2::operator/=(float s){
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

float vector2::lenth() const{
    return sqrt(x*x+y*y);
};

vector2 vector2::normalize() const{
    float len = lenth();
    if(len>0) return vector2(x/len,y/len);
    return vector2(0,0);
};

float vector2::distanceTo(const vector2 other){
    return sqrt(pow(x-other.x,2)+pow(y-other.y,2));
};

POINT vector2::to_POINT(){
    POINT p;
    p.x = this->x;
    p.y = this->y;
    return p;
};

void vector2::print(){
    printf("(%f, %f)",&x,&y);
}

vector2 vector2::rotate(float theta){
    vector2 rv;
    float rx = x*cos(theta)-y*sin(theta);
    float ry = x*sin(theta)+y*cos(theta);
    rv.x = rx;
    rv.y = ry;
    return rv;
}



bird::bird(COLORREF c, float s, vector2 initV, vector2 initP){
    color = c;
    scale = s;
    position = initP;
    velocity = initV;
    isCaptain = 0;
}

bird::~bird(){};

// 鸟群运动函数
void bird::tick(float dt, float maxSpeed, int mx, int my, int random_step){
    // 边界条件
    int boundary_force_max = 50;
    int boundary = 5;
    vector2 boundary_force(0,0);
    if(this->position.x + velocity.x*dt*2 < boundary) boundary_force.x += (boundary - this->position.x)/boundary;
    if(this->position.x + velocity.x*dt*2 > mx-boundary) boundary_force.x -= (this->position.x-(mx-boundary))/boundary;
    if(this->position.y + velocity.y*dt*2 < boundary) boundary_force.y += (boundary - this->position.y)/boundary;
    if(this->position.y + velocity.y*dt*2 > my-boundary) boundary_force.y -= (this->position.y - (my-boundary))/boundary;

    boundary_force = boundary_force.normalize()*boundary_force_max;
    if(boundary_force.lenth()!=0) velocity = velocity + boundary_force*dt;
    else velocity = velocity + acceleration*dt;

    // 噪声
    std::normal_distribution<> dist(0,1);
    vector2 noise(dist(this->gen),dist(this->gen));
    noise = noise * random_step;

    velocity = velocity + noise*dt;

    if(velocity.lenth() > maxSpeed){
        velocity = velocity.normalize()*maxSpeed;
    }

    position = position + velocity*dt;

    // 强制边界
    if(this->position.x < 0) this->position.x = 0;
    if(this->position.x > mx) this->position.x = mx ;
    if(this->position.y < 0) this->position.y = 0;
    if(this->position.y > my) this->position.y = my;
}

vector2 bird::getPos() {return position;}

vector2 bird::getV() {return velocity;}

// 鸟思考函数
void bird::think(std::vector<bird*> &boids,int sep_range, int par_range, int agg_range, float s_weight, float p_weight, float a_weight, float maxForce, float minForce, bool is_pressed, vector2 mouse_position, int mouse_range, int chase_speed)
{
    vector2 sepVector(0,0),parVector(0,0),aveVector(0,0);
    int sepNum=0,parNum=0,aveNum=0;
    if(boids.size()==1) return;
    for(bird *other:boids){
        if(other == this) continue;

        float distance = this->position.distanceTo(other->position);

        // 分离速度 采用5*e^(-disance)
        if(distance < sep_range){
            vector2 away = (this->position-other->position).normalize();
            away = away*(10.0f/pow(M_E,distance));
            sepVector = sepVector + away;
            sepNum++;
        }

        // 聚合速度 求出周围鸟群中心
        if(distance < par_range){
            parVector = parVector + other->position;
            parNum++;
        }

        // 对齐速度 求出周围鸟群中心
        if(distance < agg_range){
            aveVector = aveVector + other->velocity;
            aveNum++;
        }
    }

    if(parNum>0) parVector / parNum;
    vector2 desiredparVector = parVector - this->position;

    aveVector = (aveNum>0)?aveVector / aveNum:aveVector;

    int needPar = 1;
    vector2 chase_force(0,0);

    if(is_pressed){
        float distance = (this->position - mouse_position).lenth();
        // 聚集
        if(distance > mouse_range){
            needPar = 0;
            chase_force = (mouse_position - this->position).normalize() * chase_speed;
        }
        // 环绕
        else if(distance >= mouse_range * 0.5 && distance <= mouse_range * 1.5){
            std::normal_distribution<> dist(0,0.5);
            chase_force = (mouse_position - this->position).normalize().rotate(float(M_PI_2 + dist(this->gen)/2)) * chase_speed;
            needPar = 0;
        }
        else if(distance < mouse_range){
            chase_force = (mouse_position - this->position).normalize() * chase_speed * -1;
            needPar = 0;
        }
    }

    p_weight = 0;
    // 总和加权计算与 最大最小转向力限制 最大速度限制
    vector2 delta = sepVector*s_weight + parVector*p_weight*needPar + aveVector*a_weight;
    delta.limit(maxForce);
    delta = delta * ((delta.lenth() >= minForce)?1:0);
    if(!needPar) delta = delta.normalize() * chase_force.lenth() / 2;
    delta = delta + chase_force;
    acceleration = delta;
}

// 渲染函数
void bird::render(const std::vector<vector2> &shape, int point_num){
    POINT* dot_list;
    dot_list = new POINT[point_num];
    vector2 face = this->getV().normalize();
    for(int i=0;i<point_num;i++){
        vector2 dots;
        dots.x = (shape[i].x * face.x - shape[i].y * face.y) * this->scale + this->position.x;
        dots.y = (shape[i].x * face.y + shape[i].y * face.x) * this->scale + this->position.y;
        dot_list[i] = dots.to_POINT();
    }
    setfillcolor(this->color);
    solidpolygon(dot_list,point_num);
    delete [] dot_list;
}

void bird::update(const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset){
    std::vector<double> distance;
    for(auto *i:boids){
        if(i == this) distance.push_back(0.);
        else distance.push_back(position.distanceTo(i->getPos()));
    }
    vector2 s = Rule::Separation(this, boids, distance, eset, bset);
    vector2 a = Rule::Alignment(this, boids, distance, eset, bset);
    vector2 c = Rule::Cohesion(this, boids, distance, eset, bset);
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

void bird::tick_v2_0(const EnvSetting &eset, const BirdSetting &bset){
    if(acceleration.lenth() > bset.f_max){
        acceleration = acceleration.normalize() * bset.f_max;
    }
    if(acceleration.lenth() < bset.f_min){
        acceleration = vector2(0.,0.);
    }

    velocity += acceleration * eset.DT * 60 / eset.RENDER_FPS;

    if(velocity.lenth() > bset.v_max){
        velocity = velocity.normalize() * bset.v_max;
    }

    if(position.x < 0) position.x = 0;
    if(position.y < 0) position.y = 0;
    if(position.x > eset.MX) position.x = eset.MX;
    if(position.y > eset.MY) position.y = eset.MY;

    position += velocity * eset.DT * 60 / eset.RENDER_FPS;
};

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
        return (self->getPos() - sep_vector).normalize() * Birdmath::SIP(10., (self->getPos() - sep_vector).lenth()) * bset.c_w;
    };

    vector2 Cohesion(bird* self, const std::vector<bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset){
        vector2 coh_vector(0, 0);
        int coh_count = 0;
        for(int i = 0;i < boids.size();i++){
            if(boids[i] == self) continue;
            if(distance[i] < bset.c_r){
                coh_vector += boids[i]->getPos();
                coh_count++;
            }
        }
        if(coh_count != 0) coh_vector /= coh_count;
        return (coh_vector - self->getPos()).normalize() * ((self->getPos() - coh_vector).lenth() + 1) * bset.c_w;
    };

    vector2 Alignment(bird* self, const std::vector<bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset){
        vector2 ali_vector(0,0);
        int ali_count = 0;
        for(int i = 0;i < boids.size();i++){
            if(boids[i] == self) continue;
            if(distance[i] < bset.a_r){
                ali_vector += boids[i]->getV();
                ali_count++;
            }
        }
        if(ali_count != 0) ali_vector /= ali_count;
        return ali_vector - self->getV() * bset.a_w;
    };

    vector2 ChaseMouse(bird* self, const EnvSetting &eset, const BirdSetting &bset){
        if(eset.IS_LEFTBUTTON_DOWN){
            double d = self->getPos().distanceTo(eset.MOUSE_POSITION);
            if(d > 1.5*bset.mouse_r){
                return eset.MOUSE_POSITION - self->getPos();
            }
            else if(d <= 1.5 * bset.mouse_r && d >= 0.5 * bset.mouse_r){
                vector2 chase = eset.MOUSE_POSITION - self->getPos();
                chase.rotate(M_PI_2);
                return chase;
            }
            else{
                return (self->getPos() - eset.MOUSE_POSITION).normalize() * Birdmath::SIP(bset.mouse_r, d);
            }
        }
        else{
            return vector2(0.,0.);
        }
    };

    vector2 AvoidBoundary(bird* self, const EnvSetting &eset){
        vector2 boundary_force;
        if(self->getPos().x + self->getV().x*eset.DT*2 < eset.BOUNDARY) boundary_force.x += (eset.BOUNDARY - self->getPos().x)/eset.BOUNDARY;
        if(self->getPos().x + self->getV().x*eset.DT*2 > eset.MX-eset.BOUNDARY) boundary_force.x -= (self->getPos().x-(eset.MX-eset.BOUNDARY))/eset.BOUNDARY;
        if(self->getPos().y + self->getV().y*eset.DT*2 < eset.BOUNDARY) boundary_force.y += (eset.BOUNDARY - self->getPos().y)/eset.BOUNDARY;
        if(self->getPos().y + self->getV().y*eset.DT*2 > eset.MY-eset.BOUNDARY) boundary_force.y -= (self->getPos().y - (eset.MY-eset.BOUNDARY))/eset.BOUNDARY;
        return boundary_force.normalize() * eset.BOUNDARY_FORCE;
    }
}