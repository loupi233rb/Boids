
template<typename T>
CrossNode<T>::CrossNode(){
    data = nullptr;
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

template<typename T>
T* CrossList<T>::get(int r, int c){
    // 无效坐标
    if(r<0 || r>=rows || c<0 || c>=cols) return nullptr;
    CrossNode<T> *p,*p_pre;
    for(p=row[r],p_pre=p;p != nullptr && p->col < c;p_pre = p,p = p->right);
    // 存在直接返回节点数据指针
    if(p!=nullptr && p->col == c){
        return p->data;
    }
    // 不存在新建节点并返回数据指针
    else{
        CrossNode<T> *np;
        T *data_ptr;
        np = new CrossNode<T>;
        data_ptr = new T;
        np->row = r;
        np->col = c;
        np->data = data_ptr;
        // 插入行链表

        if(p == nullptr){
            // 插入到行尾
            p_pre->right = np;
            np->left = p_pre;
        }
        else{
            // 插入到行中间
            np->left = p_pre;
            np->right = p;
            p_pre->right = np;
            p->left = np;
        }
        // 插入列链表
        CrossNode<T> *q, *q_pre;
        for(q=col[c], q_pre=q;q != nullptr && q->row < r;q_pre = q, q = q->down);

        if(q == nullptr){
            // 插入到列尾
            q_pre->down = np;
            np->up = q_pre;
        }
        else{
            // 插入到列中间
            np->up = q_pre;
            np->down = q;
            q_pre->down = np;
            q->up = np;
        }
        data_num+=1;
        return data_ptr;
    }
}

template<typename T>
void CrossList<T>::clear() {
    // 遍历所有行，删除数据节点
    for (int i = 0; i < rows; i++) {
        CrossNode<T>* current = row[i]->right;  // 跳过行头节点
        while (current != nullptr) {
            CrossNode<T>* next = current->right;
            if (current->data != nullptr) {
                delete current->data;
            }
            // 从列链表中移除该节点
            int colIndex = current->col;
            CrossNode<T> *colp, *colpre;
            for(colp=col[colIndex],colpre=colp;colp!=nullptr && colp!=current;colpre=colp,colp=colp->down);
            if(colp == current){
                colpre->down = colp->down;
                if (colp->down != nullptr) {
                    colp->down->up = colpre;
                }
            }
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

template <typename T>
CrossList<T>::~CrossList(){
    this->clear();
}

