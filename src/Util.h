#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>
#include <string>

int setSocketNonBlocking(int fd);
void setSocketNodelay(int fd);
void setSocketNoLinger(int fd);
void shutDownWR(int fd);
int socket_bind_listen(int port);
#endif