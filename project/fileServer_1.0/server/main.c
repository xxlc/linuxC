#include "head.h"
#include "task_que.h"
#include "factory.h"
#include "thread_worker.h"
#include "tcp_init.h"
#include "epoll_op.h"
#include "transFile.h"
#include "user.h"
#include "server_function.h"

#define THREAD_NUM 10
#define QUEUE_LEN  30
int exitFds[2];
void sigFunc(int signum){
	write(exitFds[1], &signum, 1);
}
int main(int argc, char* argv[]){
	if(argc!=3){printf("./main IP PORT\n");return -1;}
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
	//user list
	List_t userList;
	listInit(&userList);
	userInforLoad(&userList, (char*)"./conf/userInformation");
	pListNode_t pListNode;
	//中间变量，用于接受数据包
	int newFd;
	int ret;
	size_t packetLen;
	char *packetBuf;//数据包变量
	pNode_t newNode;
	//主线程功能：监听新连接，退出机制，接收socket请求数据包
	while(1){
		readFdNum=epoll_wait(epfd, events, 20, -1);
		for(i=0; i<readFdNum; i++){
			if(sfd==events[i].data.fd){
				//新连接
				newFd = accept(sfd, NULL, NULL);
				printf("new client connect\n");
				int type;
				ret = recvCycle(newFd, (char*)&type, sizeof(int));
				if(-1==ret){close(newFd);continue;}
				printf("type=%d\n",type);
				switch(type){
					case 1://login verify
						ret = userLoginVerify(&userList, newFd);
						printf("userLoginVerify, ret=%d\n", ret);
						break;
					case 2://login register
						ret = userLoginRegister(&userList, newFd);
						printf("userLoginRegister, ret=%d\n", ret);
						break;
					default:ret = -1; break;
				}
				if(-1==ret){
					int error = 0;
					send(newFd, (void*)&error, sizeof(int), 0);
					close(newFd);
					continue;
				}
				epollAdd(epfd, newFd); 
			}else if(events[i].data.fd==exitFds[0]){
				//退出机制
				userInforSave(&userList,(char*)"./conf/userInformation");
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
				newFd =events[i].data.fd; 
				ret = recvCycle(newFd, (char*)&packetLen, sizeof(size_t));
				if(-1==ret){
					//下线
					printf("client exit\n");
					userOffline(&userList, newFd);
					epollDel(epfd, newFd);
					close(newFd);
					continue;
				}
				printf("-----------new task arrived---------------\n");
				packetBuf = (char*)calloc(1, packetLen+1);
				ret = recvCycle(newFd, packetBuf, packetLen);
				if(-1==ret){continue;}
				listQueryByFd(&userList, newFd, &pListNode);
				newNode = (pNode_t)calloc(1, sizeof(Node_t));
				ret = parsePacket(&newNode->task, packetBuf);
				if(-1==ret){free(newNode);continue;}
				newNode->pUser = &pListNode->user;
				pthread_mutex_lock(&pq->mutex);
				enQueue(pq, newNode);
				pthread_mutex_unlock(&pq->mutex);
				pthread_cond_signal(&factory.cond);
			}
		}
	}
	return 0;
}
