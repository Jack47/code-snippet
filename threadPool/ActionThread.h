class CActionThread{
public:
    bool attachTask(CThreadTask* task, bool needFree=false);//调用这个函数的前提是，调用方从线程池里拿到了 ActionThread线程。此时肯定是ActionThread空闲的状态。所以attachTask 的时候，mutext基本不会阻塞的。但是当action里面等待挂接的时候，就开销会大点？
    
protected:
    //只有线程池才能调用它的构造函数
    CActionThread(CThreadPool* threadPool, int serial);
    virtual ~CActionThread();
    //停止线程运行
    void stop();
    //CActionThread干活的线程
    void action();
private:
    //给pthread_create 用的
    static void run(CActionThread* );

    pthread_t m_thread;
    pthread_cond_t m_attachCond;
    pthread_mutex_t m_attachMutext;

    CThreadTask* m_task;   //这个变量会有 attachTask[调用方］和  action线程里面进行同时修改，所以需要互斥
    CThreadPool* m_threadPool; //线程池主人是谁。内部修改，所以不需要互斥

    bool m_threadRunning; //是否Action线程正常初始化，然后开始跑了。 利用pthread，就是直接创建了线程，线程就开始跑了，没有初始化这么一说，所以没有叫m_init这个变量

    bool m_freeTask;     //这个变量会有 attachTask[调用方］和  action线程里面进行同时修改，所以需要互斥
};
