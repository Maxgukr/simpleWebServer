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
    EventLoop();
    ~EventLoop();

    void loop();

    void quit();
    /* TODO:
    void shutdown(shared_ptr<Channel> channel)
    {
        shutDown(channel->getFd(), SHUT_WR);
    }*/
    void removeFromPoller(std::shared_ptr<Channel> channel);

    void updatePoller(std::shared_ptr<Channel> channel);

    void addToPoller(std::shared_ptr<Channel> channel);

private:
    bool looping_;
    std::shared_ptr<Epoll> poller_;
    bool quit_;
    bool eventHandling_;
    //mutable MutexLock mutex_;
};
#endif