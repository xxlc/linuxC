#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX 64
struct msgNode{
	char usrname[10];
	char time[30];
	char data[1024];
};
struct msgQueueNode{
	long mtype;
	struct msgNode msg;
};
