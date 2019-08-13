#include "condition.h"
#include "mutex.h"
#include <assert.h>
#include <queue.h>

//有界阻塞队列　用于生产者消费者队列　
namespace mw
{
template<typename T>
class BoundedBlockingQueue final
{
    public:
    explicit BoundedBlockingQueue(int maxSize)
    : mutex_(),
      notEmpty(mutex_),
      notFull(mutex_),
      queue_(maxSize)
    {}

    void put(const T& x)//任务入队
    {
        MutexLockGuard lock(mutex_);//作用域保证自动初始化和释放
        while(queue_.full())//任务队列现在是满的
        {
            notFull_.wait();//调用条件变量等待
        }
        assert(!queue_.full());
        queue_.push_back(x);
        notEmpty_.notify();//条件变量通知队列非空
    }

    T take()//取一个任务
    {
        MutexLockGuard lock(mutex_);
        while(queue_.empty())
        {
            notEmpty_.wait();//等待队列非空信号
        }
        assert(!queue_.empty());
        T front(std::move(queue_.front()));
        queue_.pop_front();
        notFull_.notify();
        return std::move(front);
    }

    bool full() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.full();
    }

    bool empty() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.empty();
    }

    size_t size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

    size_t capacity() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.capacity();
    }

    private:
    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    std::queue<T> queue_;

    
};
}