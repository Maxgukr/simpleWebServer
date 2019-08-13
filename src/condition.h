#ifndef CONDITION_H
#define CONDITION_H

#include <pthread.h>
#include <exception>
#include "mutex.h"


class Condition final
{
    public:
    explicit Condition(MutexLock& mutex) : mutex_(mutex)//使用explicit修饰的构造函数，只能直接初始化，不能在可能发生隐式转换的拷贝形式初始化
    {
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);//表明资源可用
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);//表明状态变化
    }

    private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};
// end of namespace mw
#endif
