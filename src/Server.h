#ifndef SERVER_H
#define SERVER_H

#include "EventLoop.h"
#include "ThreadPool.h"
#include "Channel.h"
#include "Task.h"
#include <memory>

class Server
{
public:
    Server(EventLoop *loop, int threadNum, int maxQueueSize, int port);
    ~Server();
    void start();
    void handNewConn(); //处理新的连接
    void handThisConn(); //处理当前连接

private:
    EventLoop* loop_; //主线程事件循环
    int threadNum_; //线程池线程数量
    int maxQueueSize_; //任务队列长度
    ThreadPool<task>* thread_pool_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_; //接受连接的通道
    int port_;
    int listenFd_; //监听描述符
    static const int MAXFDS = 100000; //最大文件描述符个数
};
#endif