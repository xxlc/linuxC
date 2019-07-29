#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/time.h>
#include <strings.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <sys/sendfile.h>

#define BUSY 0
#define LEISURE 1
#define LISTEN_MAX 10

typedef struct{
	int pid;
	int childFd;
	int stats;
}proInfo_t;

int sendFd(int, int);
int recvFd(int, int*);
int serverTcpInit(int *, char *, unsigned short);
int create_child_process(proInfo_t*, int); 
int process_work(int);
int epollAdd(int epfd, int fd);
int epollDel(int, int);
int sendFileByMmap(int targetFd, char *fileName); 
