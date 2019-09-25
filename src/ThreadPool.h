#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <deque>  
#include <vector>
#include <pthread.h>
#include <functional>      
#include "mutex.h"
#include "condition.h"

const int THREADPOOL_INVALID = -1;
const int THREADPOOL_LOCK_FAILURE = -2;
const int THREADPOOL_QUEUE_FULL = -3;
const int THREADPOOL_SHUTDOWN = -4;
const int THREADPOOL_THREAD_FAILURE = -5;
const int THREADPOOL_GRACEFUL = 1;


template<typename T>
class ThreadPool final
{
public:
    //typedef std::function<void ()> Task;
    
    ~ThreadPool();

    size_t queueSize() const; //获取任务队列中任务数量数量

    void addTask(T *task); //往线程池中添加任务

    //static ThreadPool* createThreadPool(int numThreads = 5, size_t maxQueueSize=100);
    ThreadPool(int numThreads = 5, int maxQueueSize=100);

    //ThreadPool<T>* createThreadPool(int numThreads, int maxQueueSize);

    void start(); //开启线程池　完成初始化工作
private: 
    
    bool isFull() const; //需要加锁保护 　判断任务队列是否是满的
    static void* runInThread(void *args); //线程池的运行函数
    T* take(); //获取任务
   
    void stop(); //销毁线程池

    mutable MutexLock mutex_;
    //
    Condition notEmpty_; //条件变量　判断任务队列不为空
    Condition notFull_; //条件变量　判断任务队列是否满

    std::vector<pthread_t> threads_; //线程池容器
    std::deque<T *> queue_; //任务队列容器
    int numThreads_;
    size_t maxQueueSize_; //任务队列最大容量
    bool running_; //线程池运行状态标记
    
};
 
#endif