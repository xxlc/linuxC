#include "head.h"
#include "task_que.h"
#include "factory.h"
#include "thread_worker.h"
#include "tcp_init.h"
#include "epoll_op.h"
#include "transFile.h"
#include "net_protocol.h"

#define THREAD_NUM 10
#define QUEUE_LEN  30
int exitFds[2];
void sigFunc(int signum){
	write(exitFds[1], &signum, 1);
}
int main(int argc, char* argv[]){
	if(argc!=3){
		printf("./main IP PORT\n");
		return -1;
	}
	pipe(exitFds);
	while(fork()){
		signal(SIGQUIT, sigFunc);
		int status;
		wait(&status);
		if(WIFEXITED(status)){
			printf("server closed\n");
			exit(0);
		}
	}
	close(exitFds[1]);
	//mysql connect
	//tcp init
	int sfd;
	serverTcpInit(&sfd, argv[1], (unsigned short)atoi(argv[2]));
	//epoll init
	int epfd = epoll_create(1);
	epollAdd(epfd, sfd);
	epollAdd(epfd, exitFds[0]);
	struct epoll_event *events;
	events = (struct epoll_event*)calloc(20, sizeof(struct epoll_event));
	int readFdNum, i;
	//thread pool start
	Factory_t factory;
	factoryInit(&factory, QUEUE_LEN, THREAD_NUM);
	factoryStart(&factory, (char*)"factory");
	pQue_t pq = &factory.que;
	//中间变量，用于接受数据包
	int clientFd;
	int ret;
	size_t packetLen;
	char packetBuf[1000];//数据包变量
	pNode_t newNode;
	//主线程功能：监听新连接，退出机制，接收socket请求数据包
	while(1){
		readFdNum=epoll_wait(epfd, events, 20, -1);
		for(i=0; i<readFdNum; i++){
			if(sfd==events[i].data.fd){
				//新连接
				clientFd = accept(sfd, NULL, NULL);
				epollAdd(epfd, clientFd);
				printf("new client connect\n");
			}else if(events[i].data.fd==exitFds[0]){
				//退出机制
				for(int i=0; i<factory.threadNum; i++){
					pthread_cancel(factory.pthid[i]);
				}
				for(int i=0; i<factory.threadNum; i++){
					pthread_join(factory.pthid[i], NULL);
				}
				printf("main thread exited\n");
				exit(0);
			}else{
				//接收socket请求数据包
				clientFd = events[i].data.fd;
				ret = recvCycle(clientFd, (char*)&packetLen, sizeof(size_t));
				if(-1==ret){continue;}
				ret = recvCycle(clientFd, packetBuf, packetLen);
				if(-1==ret){continue;}
				newNode = (pNode_t)calloc(1, sizeof(Node_t));
				ret = parsePacket(&newNode->task, packetBuf, packetLen);
				if(-1==ret){free(newNode);continue;}
				newNode->epollFd = epfd;
				newNode->clientFd = clientFd;
				epollDel(epfd, clientFd);
				pthread_mutex_lock(&pq->mutex);
				enQueue(pq, newNode);
				pthread_mutex_unlock(&pq->mutex);
				pthread_cond_signal(&factory.cond);
			}
		}
	}
	return 0;
}
