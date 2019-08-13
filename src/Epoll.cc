#include "Epoll.h"
#include <sys/epoll.h>
#include <errno.h>
#include <netinet/in.h>
#include <string>
#include <queue>
#include <arpa/inet.h>
#include <assert.h>

const int EVENTSUM = 4096;
const int EPOLLWAIT_TIME = 10000;

typedef shared_ptr<Channel> SP_Channel;

Epoll::Epoll():
    epollFd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(EVENTSUM)
    {
        assert(epollFd_ > 0);
    }

Epoll::~Epoll()
{

}

// 注册新描述符
void Epoll::epoll_operate(SP_Channeel req, int op)
{
    int fd = req->getFd(); //获得对应的文件描述符
    //创建新的　epoll_event 对象
    struct epoll_event event;
    event.data.fd = fd;
    event.events = req->getEvents();

    //关联到容器中
    if(op == EPOLL_CTL_ADD && fd2Channel_.find(fd) == fd2Channel_.end())//添加操作
        fd2Channel_[fd] = req;

    if(epoll_ctl(epollFd_, op, fd, &event) < 0) //注册/修改/删除 事件
    {
        perror("epoll_add error!");
        fd2Channel_.erase(fd);
    }

    if(op == EPOLL_CTL_DEL)
    {
        fd2Channel_.erase(fd);//删除
    }
}

std::vector<SP_Channel> Epoll::poll()
{
    while(true)
    {
        int eventNums = epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
        if(eventNums < 0)
            perror("epoll_wait error!");
        
        std::vector<SP_Channel> req_data = getEventReq(eventNums);
        if(req_data.size() > 0)
            return req_data;
    }
}

std::vector<SP_Channel> Epoll::getEventReq(int eventNUms)
{
    std::vector<SP_Channel> req_data;
    for(int i=0;i<eventNums;i++)
    {
        int fd = events_[i].data.fd; //获取活跃事件的描述符

        SP_Channel cur_channel = fd2Channel_[fd]; //得到对应的channel

        if(cur_channel) //存在
        {
            cur_channel->setRevents(events_[i].events);
            cur_channel->setEvents(0);
            req_data.push_back(cur_channel);
        } 
        else
        {
            perror("current channel is invalid!");
        }
        
    }
    return req_data;
}