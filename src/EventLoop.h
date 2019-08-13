#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "mutex.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>
#include <memory>
#include <functional>
#include <assert.h>

class EventLoop
{
public:
    typedef std::function<void()> Functor;
    EventLoop():
    looping_(false),
    poller_(new Epoll()),
    //wakeupFd_(createEventfd()),
    quit_(false),
    eventHandling_(false)
    {

    }

    ~EventLoop()
    { }

    void loop()
    {
        assert(!looping_);
        looping_ = true;
        quit_ = false;
        std::vector<SP_Channel> ret;
        while (!quit_)
        {
            //cout << "doing" << endl;
            ret.clear();
            ret = poller_->poll();
            eventHandling_ = true;
            for (auto &it : ret)
                it->handleEvents();
            eventHandling_ = false;
        }
    looping_ = false;
    }

    void quit()
    {
        quit_ = true;
    }
    /* TODO:
    void shutdown(shared_ptr<Channel> channel)
    {
        shutDown(channel->getFd(), SHUT_WR);
    }*/
    void removeFromPoller(std::shared_ptr<Channel> channel)
    {
        poller_->epoll_operate(channel, EPOLL_CTL_DEL);
    }
    void updatePoller(std::shared_ptr<Channel> channel)
    {
        poller_->epoll_operate(channel, EPOLL_CTL_MOD);
    }
    void addToPoller(std::shared_ptr<Channel> channel)
    {
        poller_->epoll_operate(channel, EPOLL_CTL_ADD);
    }

private:
    bool looping_;
    std::shared_ptr<Epoll> poller_;
    bool quit_;
    bool eventHandling_;
    //mutable MutexLock mutex_;
};
#endif