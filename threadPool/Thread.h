#include <pthread.h>

class CThread {
    //这样就可以用函数，来调用类里面的非共有的成员变量了
    friend void threadRun(CThread* pThread);
public:
    CThread(bool detach=false);
    virtual ~CThread();
    
    int start();
    void join();

protected:
    virtual run() = 0;
    virtual onExit();
private:
    pthread_t _thread;
    bool _isRunning;
    bool _detachable;
    pthread_attr_t _threadAttr;
};
