#include "threadPool/Thread.h"

void threadRun(CThread* pThread){
    pThread->run();
    pThread->onExit();
}
CThread::CThread(bool detach) : _isRunning(false) {
    _detachable = detach;
    pthread_attr_init(&_threadAttr);
    if(_detachable){
        pthread_attr_setdetachstate(&_threadAttr, PTHREAD_CREATE_DETACHED);
    }
    else {//default
        pthread_attr_setdetachstate(&_threadAttr, PTHREAD_CREATE_JOINABLE);
    }
}
CThread::~CThread(){
    pthread_attr_destroy(&_threadAttr);
}
int CThread::start(){
    if(_isRunning){
        return true;
    }
    int ret = pthread_create(&_thread, &_threadAttr, threadRun, this);
    if(ret==0){
        _isRunning = true;
    }
    return ret;
}
int CThread::join(){
    int ret = pthead_join(_thread,NULL);
    return ret;
}
