#include <bird.h>
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
void bird::think(std::vector<bird*> &boids, int sep_range, int par_range, int agg_range, float s_weight, float p_weight, float a_weight, float maxForce, float minForce, bool is_pressed, vector2 mouse_position, int mouse_range, int chase_speed)
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
void bird::render(vector2* shape, int point_num){
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

// 跟随鼠标