#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <deque>  
#include <vector>
#include <pthread.h>
#include <functional>
#include <assert.h>
#include <stdio.h>
#include <exception>      
#include "mutex.h"
#include "condition.h"

const int THREADPOOL_INVALID = -1;
const int THREADPOOL_LOCK_FAILURE = -2;
const int THREADPOOL_QUEUE_FULL = -3;
const int THREADPOOL_SHUTDOWN = -4;
const int THREADPOOL_THREAD_FAILURE = -5;
const int THREADPOOL_GRACEFUL = 1;

/*
模板类的成员函数实现应该放在头文件中
因为模板类本质上不是类，只有实例化之后才会编译生成.o文件
*/

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

template<typename T>
ThreadPool<T>::ThreadPool(int numThreads, int maxQueueSize)
    :mutex_(),
    notEmpty_(mutex_),
    notFull_(mutex_),
    numThreads_(numThreads), //初始化线程组
    maxQueueSize_(maxQueueSize), //初始化队列
    running_(false)
{
    printf("creating thread pool...\n");
    //start();
    //printf("create thread pool successful\n");
}

template<typename T>
ThreadPool<T>::~ThreadPool()
{
    if(running_)
    {
        queue_.clear();//清空任务队列
        stop();
    }
}

template<typename T>
void ThreadPool<T>::start()
{
    assert(threads_.empty());
    running_ = true; //开启开启线程池
    threads_.reserve(numThreads_); 
    //threads_ = (pthread_t*)malloc(sizeof(pthread_t) * numThreads_);
    for(int i=0;i<numThreads_;i++)
    {
        //printf("creating thread %d\n", i);
        pthread_t p;
        pthread_create(&p, NULL, runInThread, this);
        threads_.push_back(p);
    }
    printf("thread number %lu\n", threads_.size());
    for(int i=0;i<numThreads_;i++)
    {
        printf("thread id %lu\n", threads_[i]);
    }
    printf("create thread pool successful\n");
}

template<typename T>
void ThreadPool<T>::stop()
{
    {
    MutexLockGuard lock(mutex_);
    running_ = false;
    notEmpty_.notifyAll(); //通知所有线程，　要清空线程池了
    }
    printf("clearing thread pool...\n");
    for(auto& thr:threads_)
    {
        pthread_join(thr, NULL);
        printf("close thread %lu\n",pthread_self());
    }
}

template<typename T>
bool ThreadPool<T>::isFull() const
{
    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

template<typename T>
void ThreadPool<T>::addTask(T *task)//传入任务时，采用std::bind绑定具体函数，可以指定函数参数，绑定时给定默认参数
{
    printf("adding task\n");
    if (threads_.empty())
    {
        //task();//线程为空，直接执行
        fprintf(stderr, "exception caught in ThreadPool: thraed pool is empty!\n");
        abort();
    }
    else
    {
        printf("add task to thread %lu\n", pthread_self());
        MutexLockGuard lock(mutex_);
        while(isFull())//此函数在锁的作用域下
        {
            printf("queue is full waiting...\n");
            notFull_.wait();//任务队列已经满了，需要等待
        }
        assert(!isFull());
        queue_.push_back(task);
        notEmpty_.notify();//通知线程，队列中有任务了
        printf("queue size %lu\n", queue_.size());
    }
}

template<typename T>
T* ThreadPool<T>::take()
{
    //从任务队列中取数据
    MutexLockGuard lock(mutex_);
    while(queue_.empty() && running_)
    {
        notEmpty_.wait();//等待任务队列中有数据，任务队列为空则线程挂起在此状态
    }
    T *task=NULL;
    if (!queue_.empty()) //队列不空
    {
        task = queue_.front();
        queue_.pop_front();
        if(maxQueueSize_ > 0)
        {
            notFull_.notify();//通知线程，任务队列不是满的
        }
    }
    return task;
}

template<typename T>
void* ThreadPool<T>::runInThread(void* args)
{
    ThreadPool* p = (ThreadPool*)args;//转化为类指针
    try
    {
        while(p->running_)//在线程池运行期间，不断重复
        {
            T *task = p->take();
            printf("running task\n");
            if(task)
            {
                printf("running task in thread %lu\n", pthread_self());
                task->doit();//运行任务
            }
            //TODO: 任务可能为空吗？
        }
    }
    catch(const std::exception& e)
    {
        //std::cerr << e.what() << '\n';
        fprintf(stderr, "exception caught in ThreadPool\n");
        fprintf(stderr, "reason: %s\n", e.what());
        abort();
    }
}

template<typename T>
size_t ThreadPool<T>::queueSize() const
{
    MutexLockGuard lock(mutex_);
    return queue_.size();
}

#endif