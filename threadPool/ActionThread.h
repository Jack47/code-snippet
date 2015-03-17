class CActionThread{
public:
    bool attachTask(CThreadTask* task, bool needFree=false);
protected:
    CActionThread(CThreadPool* threadPool, int serial);
    virtual ~CActionThread();
    void stop();

private:
    pthread_t m_thread;
    pthread_cond_t m_attachCond;
    pthread_mutex_t m_attachMutext;

    CThreadTask* m_task;
    CThreadPool* m_threadPool;

    bool m_inited;
    bool m_exit;
    bool m_freeTask;

    static void run();
};
