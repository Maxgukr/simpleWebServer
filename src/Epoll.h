#ifndef EPOLL_H
#define EPOLL_H

#include "Channel.h"
#include <map>
#include <sys/epoll.h>
#include <memory>
#include <vector>

class Epoll //对epoll的封装
{
public:
    Epoll();
    ~Epoll();
    void epoll_operate(SP_Channel req, int op); //注册新事件
    std::vector<std::shared_ptr<Channel> > poll(); //等待事件激活
    std::vector<std::shared_ptr<Channel> > getEventReq( int event_num);
    int getEpollFd() {return epollFd_;}
private:
    static const int MAXFDS = 100000;
    int epollFd_;
    std::vector<epoll_event> events_;
    std::map<int, std::shared_ptr<Channel> > fd2Channel_;
};

#endif