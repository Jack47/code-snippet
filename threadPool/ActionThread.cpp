#include "threadPool/ActionThread.h"
//    m_exit = false; //表示现在不需要退出
//m_threadRunning = false;//表示当前线程没有跑起来
CActionThread::CActionThread(CThreadPool* threadPool, int serial): m_exit(false), m_freeTask(false), m_threadRunning(false){
    assert(threadPool!=NULL);

    m_threadPool = threadPool;
    m_attachMutex = PTHREAD_MUTEX_INITIALIZER;
    m_attachCond = PTHREAD_COND_INITIALIZER;
   

    //这里创建失败也不能抛异常，那就用一个变量来表示初始化好了没
    int ret = pthread_create(&m_thread, NULL, CActionThread::run, (void*)task);
    
    if(ret==0){//成功创建
        m_threadRunning = true;
        pthread_cond_signal(m_attachCond);//可以attach了
    }
    else {
        m_threadRunning = false;//这样下面的action线程就跑不起来了。
        m_exit = true;//启动线程失败，只能是退出状态了
    }
}
CActionThread::~CActionThread(){
    if(m_threadRunning){
        stop();
    }
    pthread_cond_destroy(&m_attachCond);
    pthread_mutex_destroy(&m_attachMutex);

}
//pthread创建线程时候的入口
void CActionThread::run(CActionThread* thread ){
    assert(thread!=NULL);
    thread->action();
}
void CActionThread::stop(){
    if(!m_threadRunning){
        return;
    }
    
    pthread_mutex_lock(&m_attachMutex);//防止其他人，比如 attachTask 挂新任务
    m_threadRunning = false;

    if(m_task!=NULL){
        m_task->stop();//先通知实际调用方，现在要停止了
        if(m_freeTask){
            delete m_task;
        }
        m_task = NULL;
    }
    pthread_cancel(&m_thread);//再关闭当前 ActionThead 启动的线程//这个也可以去掉，这样就不是立即关闭了。这样强制关闭应该没问题，此时其他地方的mutex是释放的
    pthread_mutex_unlock(&m_attachMutex);
    pthread_cond_signal&(m_attachCond);//让attachTask可以正常退出
}
//线程主体，一直执行除非接收到了stop命令
void CActionThread::action(){
    while(m_threadRunning){
        pthread_mutex_lock(&m_attachMutex);
        while(thread->m_task==NULL && m_threadRunning){//
            pthread_cond_wait(&m_attachCond, &m_attachMutex);
        }
        pthread_mutex_unlock(m_attachMutex);
        if(!m_threadRunning){//说明刚好被stop 了
            if(m_freeTask){
                delete m_task;
            }
            m_task = NULL;
            return;
        }
        m_task->run();
        
        pthread_mutex_lock(&m_attachMutex);
        assert(m_task!=NULL);
        if(m_freeTask){//跟stop的关系需要确认好
            delete m_task;
        }
        m_task = NULL;
        pthread_mutex_unlock(&m_attachMutex);
        m_threadPool->free(this);//通知线程池，自己空闲了
        pthread_cond_signal(m_attachCond);//告知生产者可以挂接了
    }
}
bool CActionThread::attachTask(CThreadTask* task, bool needFree){
    if(task==NULL){
        return true;
    }

    pthread_mutex_lock(m_attachMutex);
    assert(m_exit==true);    
    while(m_task!=NULL){//被别的task attach了
        pthread_cond_wait(&m_attachCond, &m_attachMutex);
    }
    if(m_threadRunning==false){//本actionThread已经被stop了
        return false;
    }
    m_task = task;
    m_freeTask = needFree;
    pthread_mutext_unlock(&m_attachMutex);
    pthread_cond_signal(&m_attachCond);//通知消费者action()来消费
            
    return true;
}
