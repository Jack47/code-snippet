#include "threadPool/ActionThread.h"

CActionThread::CActionThread(CThreadPool* threadPool, int serial): m_inited(true), m_exit(false), m_freeTask(false){
    assert(threadPool!=NULL);

    m_threadPool = threadPool;
    m_attachMutex = PTHREAD_MUTEX_INITIALIZER;
    m_attachCond = PTHREAD_COND_INITIALIZER;
    m_taskRunning = false;//没有指定任务
    pthread_cond_signal(m_attachCond);
}
void CActionThread::stop(){
    if(m_task!=NULL){
        m_task->stop();
    }
    pthread_mutex_lock(m_attachMutex);
    m_taskRunning = false;
    m_task = NULL;
    pthread_cond_signal(m_attachCond);
}
void CActionThread::run(CActionThread* thread ){
    assert(thread!=NULL);
    
    thread->run();//调用自己的run
    if(m_freeTask){
        delete m_task;
    }
    m_task = NULL;
   
    pthread_cond_signal(m_attachCond);
    
}
bool CActionThread::attachTask(CThreadTask* task, bool needFree){
    assert(task!=NULL);
    pthread_mutex_lock(m_attachMutex);
    
    while(m_taskRunning){//被别的task attach了
        pthread_cond_wait(&m_attachCond, &m_attachMutex);
    }
    m_taskRunning = true;//在这比较合适。如果放到下面，是放到run(),还是下面和run都放置？
    int rc = pthread_create(&m_thread, NULL, run, (void*)task);
    if(rc){
        pthread_cond_signal(m_attachCond);//可以继续attach
        m_taskRunning = false;
        pthread_mutext_unlock(&m_attachMutex);
        return false;
    }
    m_freeTask = needFree;
    pthread_mutext_unlock(&m_attachMutex);
    
    return true;
}
