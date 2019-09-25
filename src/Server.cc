#include "Server.h"
#include "Util.h"
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>

Server::Server(EventLoop* loop, int threadNum, int maxQueueSize, int port):
    loop_(loop),
    threadNum_(threadNum),
    maxQueueSize_(maxQueueSize),
    started_(false),
    acceptChannel_(new Channel(loop_)),
    //thread_pool_(ThreadPool<task>::createThreadPool(threadNum_, maxQueueSize_)),
    port_(port),
    listenFd_(socket_bind_listen(port_))
{
    acceptChannel_->setFd(listenFd_);
    if(setSocketNonBlocking(listenFd_) < 0)
    {
        perror("set socket non block failed");
        abort();
    }
}

Server::~Server()
{
    delete loop_;
    delete thread_pool_;
}

void Server::start()
{
    thread_pool_ = new class ThreadPool<task>::ThreadPool(threadNum_, maxQueueSize_); //默认的复制构造函数
   
    thread_pool_->start();
    acceptChannel_->setEvents(EPOLLIN); //读事件　水平触发
    acceptChannel_->setReadHandler(std::bind(&Server::handNewConn, this)); //处理新的连接
    acceptChannel_->setConnHandler(std::bind(&Server::handThisConn, this)); //处理本次连接
    loop_->addToPoller(acceptChannel_);

    loop_->loop(); //开启事件循环

    started_ = true;
}

void Server::handNewConn()
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in)); //初始化客户端地址
    socklen_t client_addr_len = sizeof(client_addr); //地址长度
    int accept_fd = 0;
    while((accept_fd = accept(listenFd_, (struct sockaddr*)&client_addr, &client_addr_len)) > 0) //等待客户端连接
    {
        printf("New Connection from %s %d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        // 限制服务器的最大并发连接数
        if (accept_fd >= MAXFDS)
        {
            close(accept_fd);
            continue;
        }
        //设置为非阻塞
        if (setSocketNonBlocking(accept_fd) < 0)
        {
            printf("Set non block failed!\n");
            //perror("Set non block failed!");
            return;
        }

        setSocketNodelay(accept_fd);

        //向任务队列中添加任务
        task *t = new task(accept_fd);
        thread_pool_->addTask(t); //交给线程池中的线程去执行这个连接
    }
}

void Server::handThisConn()
{ 
    loop_->updatePoller(acceptChannel_); 
}