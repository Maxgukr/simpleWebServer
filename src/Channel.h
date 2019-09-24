#ifndef CHANNEL_H
#define CHANNEL_H

#include <memory>
#include <functional>
#include <sys/epoll.h>

//前向申明

class EventLoop;

class Channel
{
private:
    typedef std::function<void()> CallBack; //回调函数
    EventLoop *loop_; //事件循环
    int fd_; //文件描述符
    unsigned int events_; //注册事件组
    unsigned int revents_; //活跃事件组

    //std::weak_ptr<HttpData> holder_; //持有该对象的对象

private:
    CallBack readHandle_; //读回调
    CallBack writeHandle_; //写回调
    CallBack connHandle_; //连接请求回调
    CallBack errorHandle_; //错误回调

public:
    Channel(EventLoop *loop):
        loop_(loop),
        fd_(0),
        events_(0)
        //revents_(0)
    {

    }

    Channel(EventLoop *loop, int fd):
        loop_(loop),
        fd_(fd),
        events_(0)
        //revents_(0)
    {
        
    }

    ~Channel();
    int getFd()
    {
        return fd_;
    }
    void setFd(int fd)
    {
        fd_ = fd;
    }

    //设置回调函数
    void setReadHandler(CallBack &&readHandler) //&&为右值引用吗？
    {
        readHandle_ = readHandler;
    }
    void setWriteHandler(CallBack &&writeHandler)
    {
        writeHandle_ = writeHandler;
    }
    void setErrorHandler(CallBack &&errorHandler)
    {
        errorHandle_ = errorHandler;
    }
    void setConnHandler(CallBack &&connHandler)
    {
        connHandle_ = connHandler;
    }

    void handleEvents()//处理事件的回调函数
    {
        events_ = 0;
        if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) //对应文件描述符被挂断
        {
            events_ = 0;
            return;
        }
        if (revents_ & EPOLLERR) //对应的文件描述符发生错误
        {
            if(errorHandle_)
            {
                errorHandle_();//执行出错回调
            }
        }
        if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) //文件描述符可读
        {
            if(readHandle_)
            {
                readHandle_();//执行读回调
            }
        }
        if (revents_ & EPOLLOUT)
        {
            if(writeHandle_)
            {
                writeHandle_();
            }
        }
        if(connHandle_)
        {
            connHandle_();//连接回调
        }
    }

    void setEvents(unsigned int ev)
    {
        events_ = ev;
    }

    void setRevents(unsigned int ev)
    {
        revents_ = ev;
    }

    unsigned int& getEvents()
    {
        return events_;
    }
};

typedef std::shared_ptr<Channel> SP_Channel;

#endif