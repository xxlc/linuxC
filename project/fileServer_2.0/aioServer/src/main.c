#include "../include/head.h"
#include "../include/task_que.h"
#include "../include/factory.h"
#include "../include/tcp_init.h"
#include "../include/epoll_op.h"

int exitFds[2];
void sigFunc(int signum){
	write(exitFds[1], &signum, 1);
}
int main(){
	FILE* fp = fopen("../conf/config", "rb");
	char args[4][20];
	fscanf(fp, "%s%s%s%s", args[0],args[1],args[2],args[3]);
#ifdef DEBUG
	printf("ip=%s,port=%s,threadNum=%s,taskMax=%s\n", args[0],args[1],args[2],args[3]);
#endif
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
	serverTcpInit(&sfd, args[0], (unsigned short)atoi(args[1]));
	//factory-thread pool
	Factory_t f;
	factoryInit(&f, atoi(args[2]), atoi(args[3]));
	factoryStart(&f);
	//epoll
	epollAdd(f.epfd, sfd);
	epollAdd(f.epfd, exitFds[0]);
	struct epoll_event *events;
	events = (struct epoll_event*)calloc(10, sizeof(struct epoll_event));
	int readFdNum, i;
	//Task Queue
	pQue_t pq = &f.que;
	pNode_t pTaskNode;
	while(1){
		readFdNum = epoll_wait(f.epfd, events, 10, -1);
		for(i=0; i<readFdNum; i++){
			if(events[i].data.fd==sfd){
				pTaskNode = (pNode_t)calloc(1, sizeof(Node_t));
				pTaskNode->clientFd = accept(sfd, NULL, NULL);
				pthread_mutex_lock(&pq->mutex);
				enQueue(pq, pTaskNode);
				pthread_mutex_unlock(&pq->mutex);
				pthread_cond_signal(&f.cond);
			}else if(events[i].data.fd==exitFds[0]){
				for(int i=0; i<f.threadNum; i++){
					pthread_cancel(f.pthid[i]);	
				}
				for(int i=0; i<f.threadNum; i++){
					pthread_join(f.pthid[i], NULL);
				}
				printf("all thread exited\n");
				exit(0);
			}else{
			
			}
		}
	}
	return 0;
}
