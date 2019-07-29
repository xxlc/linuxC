#include "head.h"
#include "task_que.h"
#include "factory.h"
#include "tcp_init.h"
#include "epoll_op.h"

int exitFds[2];
void sigFunc(int signum){
	write(exitFds[1], &signum, 1);
}
int main(int argc, char* argv[]){
	if(argc!=5){
		printf("command IP PORT THREADNUM TASKMAX\n");
		return -1;
	}
	while(fork()){
		signal(SIGUSR1, sigFunc);
		int status;
		wait(&status);
		if(WIFEXITED(status)){
			printf("server exited\n");
			exit(0);
		}
	}
	close(exitFds[1]);
	//tcp
	int sfd;
	serverTcpInit(&sfd, argv[1], (unsigned short)atoi(argv[2]));
	//factory
	Factory_t Factory;
	factoryInit(&Factory, atoi(argv[3]), atoi(argv[4]));
	factoryStart(&Factory);
	//epoll
	epollAdd(Factory.epfd, sfd);
	epollAdd(Factory.epfd, exitFds[0]);
	struct epoll_event *events;
	events = (struct epoll_event*)calloc(2, sizeof(struct epoll_event));
	int readFdNum, i;
	//Task Queue
	pQue_t pq = &Factory.que;
	pNode_t pTaskNode;
	while(1){
		readFdNum = epoll_wait(Factory.epfd, events, 2, -1);
		for(i=0; i<readFdNum; i++){
			if(events[i].data.fd==sfd){
				pTaskNode = (pNode_t)calloc(1, sizeof(Node_t));
				pTaskNode->clientFd = accept(sfd, NULL, NULL);
				pthread_mutex_lock(&pq->mutex);
				enQueue(pq, pTaskNode);
				pthread_mutex_unlock(&pq->mutex);
				pthread_cond_signal(&Factory.cond);
			}
			if(events[i].data.fd==exitFds[0]){
				for(int i=0; i<Factory.threadNum; i++){
					pthread_cancel(Factory.pthid[i]);	
				}
				for(int i=0; i<Factory.threadNum; i++){
					pthread_join(Factory.pthid[i], NULL);
				}
				printf("all thread exited\n");
				exit(0);
			}
		}
	}
	return 0;
}
