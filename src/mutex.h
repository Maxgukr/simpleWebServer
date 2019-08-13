#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <exception>


class MutexLock final
{
    public:
    MutexLock() //: holder_(0)
    {
        if(pthread_mutex_init(&mutex_, NULL) != 0)
        {
            throw std::exception();//抛出异常
        }
    }

    ~MutexLock()
    {
        //assert(holder_ == 0);//没有线程使用这个互斥量
        pthread_mutex_destroy(&mutex_);
    }

    void lock()
    {
        pthread_mutex_lock(&mutex_);
        //holder_ = CurrentThread::tid();//当前线程id
    }

    void unlock()
    {
        //holder_ = 0;
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;//仅供condition调用
    }

    private:
    friend class Condition;
    pthread_mutex_t mutex_;
    //pid_t holder_;

};

class MutexLockGuard
{
    public:
    explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex)
    {
        mutex_.lock();//调用加锁函数
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

    private:
    MutexLock& mutex_;

};


// end namespace mw

#define MutexLockGuard(x) error "Missing guard object name"

#endif
