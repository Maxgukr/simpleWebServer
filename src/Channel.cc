#include "Channel.h"

Channel::Channel(EventLoop *loop):
    loop_(loop),
    fd_(0),
    events_(0)
    //revents_(0)
{

}

Channel::Channel(EventLoop *loop, int fd):
    loop_(loop),
    fd_(fd)
    //revents_(0)
{
    
}

Channel::~Channel()
{

}
int Channel::getFd()
{
    return fd_;
}
void Channel::setFd(int fd)
{
    fd_ = fd;
}

//设置回调函数
void Channel::setReadHandler(CallBack &&readHandler) //&&为右值引用吗？
{
    readHandle_ = readHandler;
}
void Channel::setWriteHandler(CallBack &&writeHandler)
{
    writeHandle_ = writeHandler;
}
void Channel::setErrorHandler(CallBack &&errorHandler)
{
    errorHandle_ = errorHandler;
}
void Channel::setConnHandler(CallBack &&connHandler)
{
    connHandle_ = connHandler;
}

void Channel::handleEvents()//处理事件的回调函数
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

void Channel::setEvents(unsigned int ev)
{
    events_ = ev;
}

void Channel::setRevents(unsigned int ev)
{
    revents_ = ev;
}

unsigned int& Channel::getEvents()
{
    return events_;
}