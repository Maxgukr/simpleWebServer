#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/wait.h>

const int BUFFER_SIZE = 4096;

class task
{
private:
	int connfd;
	char *path = "/home/Code/build/release/WebServer";

public:
	task();
	task(int fd);
	~task();

	void response(char *message, int status);

	void response_file(int size, int status);

	void response_get(char *filename); //处理get请求

	void response_post(char *filename, char *argv); //处理post请求

	void doit();
};

#endif