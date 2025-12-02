#include "bird.h"
#include "crossList.h"

template<typename T>
CrossNode<T>::CrossNode(){
    left = nullptr;
    right = nullptr;
    up = nullptr;
    down = nullptr;
};

template<typename T>
CrossList<T>::CrossList(){
    this->row = nullptr;
    this->col = nullptr;
}

template<typename T>
CrossList<T>::CrossList(int rows, int cols){
    if(rows<=0) rows=1;
    if(cols<=0) cols=1;
    row = new CrossNode<T>*[rows];
    col = new CrossNode<T>*[cols];
    for(int i=0;i<rows;i++){
        row[i] = new CrossNode<T>;
        row[i]->col = -1;
        row[i]->row = i;
    }
    for(int i=0;i<cols;i++){
        col[i] = new CrossNode<T>;
        col[i]->col = i;
        col[i]->row = -1;
    }
    this->rows = rows;
    this->cols = cols;
    data_num = 0;
} 

template< >
bool CrossList<grid*>::set(const bird *b, int grid_size){
    int r, c;
    r = std::ceil(b->getPos().x / grid_size);
    c = std::ceil(b->getPos().y / grid_size);

    if(r<0 || r>=rows || c<0 || c>=cols) return false;

    if(row[r] == nullptr || col[c] == nullptr) return false;

    CrossNode<grid*> *p = row[r]->right;
    CrossNode<grid*> *p_pre = row[r];
    
    while(p != nullptr && p->col < c) {
        p_pre = p;
        p = p->right;
    }

    // 没有新建节点
    if(p != nullptr && p->col == c){
        p->data->push_back(b);
        return true;
    }

    // 新建节点并添加
    else{
        CrossNode<grid*> *np;
        grid *g;
        np = new CrossNode<grid*>;
        g = new grid;
        np->row = r;
        np->col = c;
        np->data = g;
        g->push_back(b);
        np->right = p;
        np->left = p_pre;
        p_pre->right = np;
        if(p != nullptr) {
            p->left = np;
        }


        // 处理列链表
        CrossNode<grid*> *q = col[c]->down;
        CrossNode<grid*> *q_pre = col[c];

        while(q != nullptr && q->row < r) {
            q_pre = q;
            q = q->down;
        }

        np->down = q;
        np->up = q_pre;
        q_pre->down = np;
        if(q != nullptr) {
            q->up = np;
        }
        data_num+=1;
        return true;
    }
}


template<>
void CrossList<grid*>::removeFromColumn(CrossNode<grid*> *node) {
    int colIndex = node->col;
    CrossNode<grid*>* prev = col[colIndex];
    CrossNode<grid*>* current = col[colIndex]->down;
    
    // 在列链表中找到该节点并移除
    while (current != nullptr && current != node) {
        prev = current;
        current = current->down;
    }
    
    if (current == node) {
        prev->down = current->down;
        if (current->down != nullptr) {
            current->down->up = prev;
        }
    }
}

template< >
void CrossList<grid*>::clear() {
    // 遍历所有行，删除数据节点
    for (int i = 0; i < rows; i++) {
        CrossNode<grid*>* current = row[i]->right;  // 跳过行头节点
        while (current != nullptr) {
            CrossNode<grid*>* next = current->right;
            
            // 删除grid对象
            if (current->data != nullptr) {
                delete current->data;
            }
            
            // 从列链表中移除该节点
            removeFromColumn(current);
            
            // 删除节点本身
            delete current;
            
            current = next;
        }
        // 重置行头节点的指针
        row[i]->right = nullptr;
    }
    
    // 重置所有列头节点的down指针
    for (int j = 0; j < cols; j++) {
        col[j]->down = nullptr;
    }
}

template < >
void CrossList<grid*>::removeBirds(){

    for(int i=0;i<this->rows;i++){
        CrossNode<grid*>* r = row[i]->right;
        while(r!=nullptr){
            if(r->data!=nullptr){
                r->data->clear();
            }
            r = r->right;
        }
    }
}

template <typename T>
CrossNode<T>* CrossList<T>::getValue(int r, int c) const
{
    if(r<0 || r>=rows || c<0 || c>=cols) return nullptr;
    CrossNode<grid*> *p;
    for(p=row[r];p->right != nullptr && p->col < c;p = p->right);
    if(p!=nullptr && p->col == c){
        return p;
    }
    else return nullptr;
}

template < >
CrossList<grid*>::~CrossList(){

    this->clear();
}


// 网格划分函数
std::vector<CrossList<grid*>*>* DivideGrid(const EnvSetting& eset, const BirdSetting& bset)
{
    std::vector<CrossList<grid*>*>* gridSet;
    gridSet = new std::vector<CrossList<grid*>*>;
    CrossList<grid*> *sg, *ag, *cg;
    sg = new CrossList<grid*>(std::ceil(eset.MX / bset.s_r), std::ceil(eset.MY / bset.s_r));
    ag = new CrossList<grid*>(std::ceil(eset.MX / bset.a_r), std::ceil(eset.MY / bset.a_r));
    cg = new CrossList<grid*>(std::ceil(eset.MX / bset.c_r), std::ceil(eset.MY / bset.c_r));
    gridSet->push_back(sg);
    gridSet->push_back(ag);
    gridSet->push_back(cg);
    return gridSet;
}

// 网格更新
void GridUpdate(std::vector<CrossList<grid*>*>* gridSet, const std::vector<bird*> &boids, const BirdSetting& bset)
{
    for(int i=0;i<3;i++) gridSet->at(i)->removeBirds();

    for(auto *i:boids){
        gridSet->at(0)->set(i, bset.s_r);
        // std::cout<<"set function"<<std::endl;
        // system("pause");
        gridSet->at(1)->set(i, bset.a_r);
        gridSet->at(2)->set(i, bset.c_r);
    }
}

std::vector<const bird*> FindNeighborGridBird(const CrossList<grid*>* grids, const bird* b, const int grid_size)
{
    int r, c;
    std::vector<grid*> all_grid;
    r = std::ceil(b->getPos().x / grid_size);
    c = std::ceil(b->getPos().y / grid_size);
    int all[2][9] = {{0,1,0,-1, 0,1, 1,-1,-1}
                    ,{0,0,1, 0,-1,1,-1, 1,-1}};
    for(int i=0;i<9;i++){
        CrossNode<grid*>* current;
        current = grids->getValue(r + all[0][i], c + all[1][i]);
        if(current != nullptr){
            all_grid.push_back(current->data);
        }
    }
    
    std::vector<const bird*> all_bird;
    for(auto *i:all_grid){
        all_bird.insert(all_bird.end(), i->begin(), i->end());
    }
    all_grid.clear();
    return all_bird;
}

void DeleteGrids(std::vector<CrossList<grid*>*>* gridSet)
{
    for(auto *i:*gridSet){
        i->clear();
        delete i;
    }
    gridSet->clear();
    delete gridSet;
}