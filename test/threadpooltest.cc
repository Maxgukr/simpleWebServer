#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "../base/ThreadPool.h"

/*测试线程池 */
void my_print1()
{
    printf("tid=%lu\n", static_cast<pid_t>(syscall(SYS_gettid)));//
}

void my_print2(const std::string& str)
{
    printf("tid=%lu, str=%s\n",static_cast<pid_t>(syscall(SYS_gettid)) , str.c_str());
}

int main()
{
    mw::ThreadPool* thread_pool = mw::ThreadPool::createThreadPool(5, 20);
    //创建10个线程的线程池
    //thread_pool.start(10);

    //添加任务 模拟生产者
    for(int i=0;i<50;i++)
        thread_pool->addTask(my_print1);
    for(int i=0;i<50;i++)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "task %d", i);
        thread_pool->addTask(std::bind(my_print2, std::string(buf)));
    }
    //模拟消费者，执行任务,任务全部执行完后，线程挂起
    while(true)
    {
        if(thread_pool->queueSize() == 0)
        {
            printf("tasks have been ran out, now exiting...");
            break;
        }
            
    }
    delete thread_pool;//开始执行析构
}