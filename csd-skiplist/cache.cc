struct LRUHandle {
    void* value;
    void (*deleter)(const Slice&, void* value);
    LRUHandle* next_hash;//用于hash 桶中碰撞时候的单向链表
    LRUHandle* next;//
    LRUHandle* prev;
    size_t charge;
    size_t key_length
};
class LRUCache {
    void Erase(const Slice&key, uint32_t hash){
       LRUHandle* h = table_.Remove(key, hash);
       if(h!=NULL){
           LRU_Remove(h);//从lru的双向链表中删掉
           Unref(h);
       }
    }
    void Release(LRUHandle* h){
        Mutex l(mutex_);
        Unref(h);
    }
    Cache::Handle* Insert(const Slice& key, uint32_t hash, void* value, size_t charge, void (*deleter)(const Slice& key, void* value)){
        LRUHandle* h = new [sizeof(LRUHandle)-1+key.size()];
        h->value = value;
        h->deleter = deleter;
        LRU_Append(h);
        h->charge = charge;
        usage_+=h->charge;//TODO
        h->key_length = key.size();
        refs=2;//one for return, one for LRUCache
        h->hash = hash;
        memcpy(key.data(),h->key_data,key.size());
        LRUHandle* old = table_.Insert(h);

        if(old!=NULL){
            LRU_Remove(h);
            Unref(old);//去掉LRUCache对它的ref
        }

        LRUHandle* e = lru_.prev;
        LRUHandle* n = NULL;
        while(usage_>capacity_&&e!=&lru){
            n = e->prev;//可以不用这样搞，n=lru.next;//Erase后，n自动被删除了
            Erase(lru_.prev->key, lru_.prev->hash);//这样效率低，不如直接talbe_.Remove(e->key, e->value); LRU_Remove(e); Unref(e);
            e = n;
        }

        return reinterpret_cast<Cache::Handle*>(h);
    }
    
private:
    void Unref(LRUHandle* h){
        assert(h->refs>=2);
        h->refs_--;
        if(h->refs_==1){
            usage_-=h->charge;
            Slice key(h->key, h->key_length);
            h->deleter(key, h->value);
            free(h);
        }
    }
    
    LRUHandle lru_;
    HandleTable table_;
    size_t usage_;
};
class HandleTable 
{
private:
    uint32_t length_;
    uint32_t elems_;
    LRUHandle** list_;
    
    void Resize(){
        uint32_t newLength = 4;
        while(newLength<=elems_){
            newLength*=2;
        }

        LRUHandle** newList = new LRUHandle*[newLength];
        memset(newList, 0, sizeof(newList[0])*newLength);

        int count = 0;
        for(int i=0;i<length_;i++){
            LRUHandle* ptr = list_[i];
            LRUHandle* nextPtr = NULL;
            uint32_t hash = 0;
            while(ptr!=NULL){
                nextPtr = ptr->next_hash;
                hash = ptr->hash;
                ptr->next_hash = newList[ hash&(newLength-1)];
                newList[hash&(newLength-1)] = ptr;
                ptr = nextPtr;
                count++;
            }
        }
        assert(count==elems_);
        delete list_;
        list_ = newList;
        length = newLength;
    }
    LRUHandle* Lookup(const Slice& key, uint32_t hash){
        return *FindPtr(key, hash);//is null or point to LRUHandle
    }
    LRUHandle* Remove(const Slice& key, uint32_t hash){
        LRUHandle** ptr = FindPtr(key, hash);
        LRUHandle* result = (*ptr);
        if(result!=NULL){
            (*ptr) = (*ptr)->next_hash;
        }
        return result;
    }
    //如果已经有值了，需要替换掉，把旧的指针返回
    //如果不在，就在桶的链表最后面插入
    LRUHandle* Insert(LRUHandle* h){
        LRUHandle** ptr = FindPtr(h->key, h->hash);
        if((*ptr)!=NULL){
            LRUHandle** old_ptr = ptr;
            h->next_hash = (*old_ptr)->next_hash;
            (*old_ptr) = h;
            return (*old_ptr);
        }
        else{
            h->next_hash = NULL;
            (*ptr) = h;
            elems_++;
            if(elems_>length_){
                Resize();
            }
            
            return NULL;
        }
        
    }

private:
    //如果在，找到的是指向这个LRUHandle*的指针
    //如果在，找到的是尾指针
    LRUHandle** FindPtr(const Slice& key, uint32_t hash){
        LRUHandle** ptr = &list_[hash&(length_-1)];
        while((*ptr)!=NULL){
            if((*ptr)->key!=key || (*ptr)->hash!=hash){
                ptr = &((*ptr)->next_hash);
            }
        }

        return ptr;
    }
};
