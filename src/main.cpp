#include "EventLoop.h"
#include "Server.h"

int main(int argc, char * argv[])
{
    int threadNum = 4;
    int port = 80;
    EventLoop mainLoop;
    Server myHttpServer(&mainLoop, threadNum, port);
    myHttpServer.start();
    mainLoop.loop();
    return 0;
}