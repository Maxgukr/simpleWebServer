#include "mutex.h"
#include "condition.h"


class CountDownLatch final
{
    public:
    CountDownLatch(int count)
     : mutex_(), 
     condition_(mutex_), 
     count_(count)
     {}

    void CountDownLatch::wait()
    {
        MutexLockGuard lock(mutex_);
        while (count_ > 0)
        {
            condition_.wait();
        }
    }

    void countDown()
    {
        MutexLockGuard lock(mutex_);
        --count_;
        if (count_ == 0)
        {
            condition_.notifyAll();
        }
    }

    int getCount() const
    {
        MutexLockGuard lock(mutex_);
        return count_;
    }

    private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};

