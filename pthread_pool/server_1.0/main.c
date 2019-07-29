#include "head.h"
#include "task_que.h"
#include "factory.h"
#include "thread_worker.h"
#include "tcp_init.h"
#include "epoll_op.h"
#include "transFile.h"

#define THREAD_NUM 3
#define QUEUE_LEN  5
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
	//文件传输功能
	Factory_t factoryTransFile;
	factoryInit(&factoryTransFile, QUEUE_LEN, THREAD_NUM, handleTransFile);
	factoryStart(&factoryTransFile, (char*)"factoryTransFile");
	//tcp init
	int sfd;
	serverTcpInit(&sfd, argv[1], (unsigned short)atoi(argv[2]));
	//epoll
	int epfd = epoll_create(1);
	epollAdd(epfd, sfd);
	epollAdd(epfd, exitFds[0]);
	struct epoll_event *events;
	events = (struct epoll_event*)calloc(10, sizeof(struct epoll_event));
	int readFdNum, i;
	//请求对象
	struct file_node fileRequest;
	bzero(&fileRequest, sizeof(fileRequest));
	strcpy(fileRequest.fileName, "file");
	pNode_t newNode;
	pQue_t pFileReqQue = &factoryTransFile.que;
	while(1){
		readFdNum=epoll_wait(epfd, events, 10, -1);
		for(i=0; i<readFdNum; i++){
			if(sfd==events[i].data.fd){
				fileRequest.clientFd = accept(sfd, NULL, NULL);
				newNode = (pNode_t)calloc(1, sizeof(Node_t));
				printf("new client connect\n");
				//newNode->pTask = function()
				newNode->pTask = (void*)&fileRequest;

				pthread_mutex_lock(&pFileReqQue->mutex);
				enQueue(pFileReqQue, newNode);
				pthread_mutex_unlock(&pFileReqQue->mutex);
				pthread_cond_signal(&factoryTransFile.cond);
			}else if(events[i].data.fd==exitFds[0]){
				for(int i=0; i<factoryTransFile.threadNum; i++){
					pthread_cancel(factoryTransFile.pthid[i]);
				}
				for(int i=0; i<factoryTransFile.threadNum; i++){
					pthread_join(factoryTransFile.pthid[i], NULL);
				}
				printf("I am child process\n");
				exit(0);
			}else{
				//for(int i=0; i<; )
			}
		}
	}
	return 0;
}
