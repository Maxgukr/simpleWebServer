#include "EventLoop.h"
#include "Server.h"

int main(int argc, char * argv[])
{
    int threadNum = 4;
    int maxQueueSize = 100;
    int port = 80;
    EventLoop mainLoop;
    Server myHttpServer(&mainLoop, threadNum, maxQueueSize, port);
    myHttpServer.start();
    mainLoop.loop();
    return 0;
}