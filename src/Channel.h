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
    Channel(EventLoop *loop);

    Channel(EventLoop *loop, int fd);

    ~Channel();
    int getFd();
    void setFd(int fd);

    //设置回调函数
    void setReadHandler(CallBack &&readHandler); //&&为右值引用
  
    void setWriteHandler(CallBack &&writeHandler);
   
    void setErrorHandler(CallBack &&errorHandler);
   
    void setConnHandler(CallBack &&connHandler);

    void handleEvents();//处理事件的回调函数
   

    void setEvents(unsigned int ev);

    void setRevents(unsigned int ev);

    unsigned int& getEvents();
};

typedef std::shared_ptr<Channel> SP_Channel;

#endif