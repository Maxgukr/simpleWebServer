#include "EventLoop.h"

EventLoop::EventLoop():
    looping_(false),
    poller_(new Epoll()),
    //wakeupFd_(createEventfd()),
    quit_(false),
    eventHandling_(false)
    {

    }

EventLoop::~EventLoop()
{ }

void EventLoop::loop()
{
    assert(!looping_);
    looping_ = true;
    quit_ = false;
    std::vector<SP_Channel> ret;
    while (!quit_)
    {
        //cout << "doing" << endl;
        ret.clear();
        ret = poller_->poll(); //返回活跃同道列表
        eventHandling_ = true;
        for (auto &it : ret)
            it->handleEvents();
        eventHandling_ = false;
    }
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
}
/* TODO:
void shutdown(shared_ptr<Channel> channel)
{
    shutDown(channel->getFd(), SHUT_WR);
}*/
void EventLoop::removeFromPoller(std::shared_ptr<Channel> channel)
{
    poller_->epoll_operate(channel, EPOLL_CTL_DEL);
}
void EventLoop::updatePoller(std::shared_ptr<Channel> channel)
{
    poller_->epoll_operate(channel, EPOLL_CTL_MOD);
}
void EventLoop::addToPoller(std::shared_ptr<Channel> channel)
{
    poller_->epoll_operate(channel, EPOLL_CTL_ADD);
}