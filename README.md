# simpleWebServer
c++实现的简单web服务器，实现GET,POST请求。Reactor+线程池

src文件中的文件树
```
├── BoundedBlockingQueue.h  //有界阻塞队列
├── Channel.h  //多路事件分发器
├── condition.h //条件变量的封装
├── CountDownLatch.h  //倒计时闩
├── Epoll.cc  //epoll结构封装，等待事件发生
├── Epoll.h
├── EventLoop.h  //事件循环, 运行在主线程，负责建立新的连接
├── mutex.h  //互斥器
├── readme.md
├── Server.cc //服务端代码 eventloop+threadpool
├── Server.h
├── Task.h  //任务请求http get&post
├── ThreadPool.cc  //静态线程池
├── ThreadPool.h
├── Util.cpp  //功能函数 socket bind listen 
└── Util.h
```

