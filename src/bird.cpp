#include "bird.h"
#include "crossList.h"
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
    gen = std::mt19937(std::random_device{}());
}

bird::~bird(){};



vector2 bird::getPos() const{return position;}

vector2 bird::getV() const {return velocity;}

// 鸟思考函数

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

void bird::update(const std::vector<bird*> &boids, const EnvSetting &eset, const BirdSetting &bset, const std::vector<bird_crossList> &gridSet){


    std::vector<bird*> sep_bird = FindNeighborGridBird(gridSet[0], this);
    std::vector<bird*> ali_bird = FindNeighborGridBird(gridSet[1], this);
    std::vector<bird*> coh_bird = FindNeighborGridBird(gridSet[2], this);


    std::vector<double> sep_distance;
    std::vector<double> ali_distance;
    std::vector<double> coh_distance;
    for(auto *i:sep_bird){
        if(i == this) sep_distance.push_back(0.);
        else sep_distance.push_back(position.distanceTo(i->getPos()));
    }
    for(auto *i:ali_bird){
        if(i == this) ali_distance.push_back(0.);
        else ali_distance.push_back(position.distanceTo(i->getPos()));
    }
    for(auto *i:coh_bird){
        if(i == this) coh_distance.push_back(0.);
        else coh_distance.push_back(position.distanceTo(i->getPos()));
    }

    vector2 s = Rule::Separation_v2(this, sep_bird, sep_distance, eset, bset) * bset.s_w;
    vector2 a = Rule::Alignment(this, ali_bird, ali_distance, eset, bset) * bset.a_w;
    vector2 c = Rule::Cohesion(this, coh_bird, coh_distance, eset, bset) * bset.c_w;
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

    // double sac_force = (s+a+c).lenth();
    // s = s.normalize() * sac_force * bset.s_w;
    // a = a.normalize() * sac_force * bset.a_w;
    // c = c.normalize() * sac_force * bset.c_w;

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

    position += velocity * eset.DT * 60 / eset.RENDER_FPS;

    if(position.x < 0) position.x = 0;
    if(position.y < 0) position.y = 0;
    if(position.x > eset.MX) position.x = eset.MX;
    if(position.y > eset.MY) position.y = eset.MY;

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
    this->color = RGB(r,g,b);
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

    vector2 Separation_v2(bird* self, const std::vector<bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset){
        vector2 sep_force(0,0);
        for(int i = 0;i<boids.size();i++){
            if(boids[i] == self) continue;
            if(distance[i] < bset.s_r && distance[i] > 1e-3f){
                sep_force += (self->getPos() - boids[i]->getPos()) / distance[i] * bset.s_r;
            }
        }
        return sep_force;
    }

    vector2 Cohesion(bird* self, const std::vector<bird*> &boids, const std::vector<double> &distance, const EnvSetting &eset, const BirdSetting &bset){
        vector2 coh_vector(0, 0);
        int coh_count = 0;
        for(int i = 0;i < boids.size();i++){
            if(boids[i] == self) continue;
            if(distance[i] < bset.c_r && distance[i] > 1e-3f){
                coh_vector += boids[i]->getPos();
                coh_count++;
            }
        }
        if(coh_count != 0) coh_vector /= coh_count;
        return coh_vector - self->getPos();
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
        if(self->getPos().x < eset.BOUNDARY) boundary_force.x += (eset.BOUNDARY - self->getPos().x)/eset.BOUNDARY;
        if(self->getPos().x > eset.MX-eset.BOUNDARY) boundary_force.x -= (self->getPos().x-(eset.MX-eset.BOUNDARY))/eset.BOUNDARY;
        if(self->getPos().y < eset.BOUNDARY) boundary_force.y += (eset.BOUNDARY - self->getPos().y)/eset.BOUNDARY;
        if(self->getPos().y > eset.MY-eset.BOUNDARY) boundary_force.y -= (self->getPos().y - (eset.MY-eset.BOUNDARY))/eset.BOUNDARY;
        return boundary_force.normalize() * eset.BOUNDARY_FORCE;
    }
}

bird_crossList::~bird_crossList() = default;

void bird_crossList::add(bird* b)
{
    int r, c;
    r = int(std::floor(b->getPos().x / grid_size));
    c = int(std::floor(b->getPos().y / grid_size));
    grid* g;
    g = this->get(r, c);
    if(g != nullptr) g->push_back(b);
};

void bird_crossList::clear_bird()
{
    for(int i=0;i<rows;i++){
        for(auto* p = row[i]; p; p = p->right){
            if (p->data) p->data->clear(); // 仅清数据，不新建节点
        }
    }
};

// 网格划分函数
std::vector<bird_crossList> DivideGrid(const EnvSetting& eset, const BirdSetting& bset)
{
    std::vector<bird_crossList> gridSet;
    bird_crossList sg(std::ceil(eset.MX / bset.s_r), std::ceil(eset.MY / bset.s_r));
    bird_crossList ag(std::ceil(eset.MX / bset.a_r), std::ceil(eset.MY / bset.a_r));
    bird_crossList cg(std::ceil(eset.MX / bset.c_r), std::ceil(eset.MY / bset.c_r));
    sg.grid_size = bset.s_r;
    ag.grid_size = bset.a_r;
    cg.grid_size = bset.c_r;
    sg.clear_bird();
    ag.clear_bird();
    cg.clear_bird();
    gridSet.push_back(sg);
    gridSet.push_back(ag);
    gridSet.push_back(cg);
    return gridSet;
}

// 网格更新
void GridUpdate(std::vector<bird_crossList> &gridSet, const std::vector<bird*> &boids, const BirdSetting& bset)
{
    for(int i=0;i<3;i++) gridSet[i].clear_bird();
    for(auto *i:boids){
        gridSet[0].add(i);
        gridSet[1].add(i);
        gridSet[2].add(i);
    }
}

// 寻找邻居
std::vector<bird*> FindNeighborGridBird(const bird_crossList &grids, bird* b)
{
    int r, c;
    std::vector<grid*> all_grid;
    r = std::ceil(b->getPos().x / grids.grid_size);
    c = std::ceil(b->getPos().y / grids.grid_size);
    int all[2][9] = {{0,1,0,-1, 0,1, 1,-1,-1}
                    ,{0,0,1, 0,-1,1,-1, 1,-1}};
    for(int i=0;i<9;i++){
        grid* current;
        current = const_cast<bird_crossList&>(grids).get(r + all[0][i], c + all[1][i]);
        if(current != nullptr){
            all_grid.push_back(current);
        }
    }
    
    std::vector<bird*> all_bird;
    for(auto *i:all_grid){
        all_bird.insert(all_bird.end(), i->begin(), i->end());
    }
    all_grid.clear();
    return all_bird;
}

// 回收内存
void DeleteGrids(std::vector<bird_crossList> &gridSet)
{
    for(auto i:gridSet){
        i.clear();
    }
    gridSet.clear();
}