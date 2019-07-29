#include "config.h"
#include <fcntl.h>

#define SIZE_2M 2<<21
struct usrNode{
	int sfd;
	char ip[16];
	unsigned short port;
	char usrname[10];
	struct usrNode *next;
};
struct statNode{
	int sfd;
	char *pStart;
	char *pCur;
};

int serverTcpInit(int *psfd, char *serverIp, unsigned short serverPort);
int usrListHeadInsert(struct usrNode **pphead, struct usrNode usr);
struct usrNode* usrListQueryByFd(struct usrNode *phead, int fd);
int usrListDeleteByFd(struct usrNode **pphead, int fd);

struct statNode stats;
void sigHandler(int signum){
	munmap(stats.pStart, SIZE_2M);
	printf("server exit.\n");
}

int main(int argc, char **argv){
	int sfd, ret;
	char serverip[] = "192.168.5.163";
	//注册退出信号
	signal(SIGINT, sigHandler);
	//mmap一个文件
	int fd = open("./record", O_RDWR);
	ftruncate(fd, SIZE_2M);
	stats.pStart=(char*)mmap(NULL, SIZE_2M, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	stats.pCur = stats.pStart;
	//开启listen状态
	ret = serverTcpInit(&sfd, serverip, 2000);
	if(-1==ret){
		return -1;
	}
	//epoll_ctl sfd
	int epfd = epoll_create(1);
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = sfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
	//epoll_wait 用参数
	struct epoll_event *events;
	events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*MAX);
	//中间变量
	struct sockaddr_in clientAddr;
	socklen_t addrlen = sizeof(struct sockaddr);
	struct usrNode usr, *phead=NULL, *pTemp;
	struct msgQueueNode message;
	int i, n, cnt=1;
	time_t now;
	while(1){
		n = epoll_wait(epfd, events, MAX, -1);
		for(i=0; i<n; i++){
			if(events[i].data.fd==sfd){
				bzero(&usr, sizeof(usr));
				bzero(&clientAddr, sizeof(clientAddr));
				usr.sfd = accept(sfd, (struct sockaddr*)&clientAddr, &addrlen);
				if(-1==usr.sfd){
					perror("accept");
					return -1;
				}
				inet_ntop(AF_INET, &clientAddr.sin_addr, usr.ip, 16);
				usr.port = ntohs(clientAddr.sin_port);
				sprintf(usr.usrname, "user%3d", cnt++);
				//发送消息给client，确定用户名
				send(usr.sfd, usr.usrname, sizeof(usr.usrname), 0);
				//添加usr.sfd到epoll_wait
				event.events = EPOLLIN;
				event.data.fd = usr.sfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, usr.sfd, &event);
				//将usr插入用户链表
				usrListHeadInsert(&phead, usr);
				printf("new client.\n");
				//加载历史聊天记录
				send()		
			}else{
				stats.sfd = events[i].data.fd;//使用中间变量
				pTemp = usrListQueryByFd(phead, stats.sfd);
				
				bzero(&message, sizeof(message));
				message.mtype = CURRENT;//转发的消息均为 #define CURRENT 2
				ret=recv(stats.sfd, message.msg.data, sizeof(message.msg.data)-1, 0);
				if(0==ret){
					printf("%s exit.\n", pTemp->usrname);
					epoll_ctl(epfd, EPOLL_CTL_DEL, stats.sfd, &event);
					close(stats.sfd);
					usrListDeleteByFd(&phead, stats.sfd);
				}
				strcpy(message.msg.usrname, pTemp->usrname);
				//添加时间戳
				time(&now);
				strcpy(message.msg.time, ctime(&now));
				message.msg.time[strlen(message.msg.time)-1] = '\0';
				//存入mmap的文件中
				strcpy(stats.pCur, message.msg.usrname);
				stats.pCur += strlen(message.msg.usrname);
				*(stats.pCur) = ' ';
				stats.pCur += 1;
				strcpy(stats.pCur, message.msg.time);
				*(stats.pCur) = '\0';
				stats.pCur += 1;
				strcpy(stats.pCur, message.msg.data);
				*(stats.pCur) = '\n';
				stats.pCur += 1;
				//发送给各个client
				pTemp = phead;
				while(pTemp){
					send(stats.sfd, &message, sizeof(struct msgQueueNode), 0);
					pTemp = pTemp->next;
				}
			}
		}
	}
	return 0;	
}

int serverTcpInit(int *psfd, char *serverIp, unsigned short serverPort){
	int ret;
    *psfd = socket(AF_INET, SOCK_STREAM, 0);
	if(*psfd==-1){
		perror("socket");
		return -1;
	}
	struct sockaddr_in serAddr;
	bzero(&serAddr, sizeof(serAddr));
	serAddr.sin_family = AF_INET;
	serAddr.sin_addr.s_addr = inet_addr(serverIp);
	serAddr.sin_port = htons(serverPort);
	ret = bind(*psfd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
	if(-1==ret){
		perror("bind");
		return -1;
	}
	listen(*psfd, MAX);
	return 0;
}
int usrListHeadInsert(struct usrNode **pphead, struct usrNode usr){
	struct usrNode *newNode = (struct usrNode*)malloc(sizeof(struct usrNode));
	*newNode = usr;
	if(NULL==*pphead){
		*pphead = newNode;
	}else{
		newNode->next = *pphead;
		*pphead = newNode;
	}
	return 0;
}
struct usrNode* usrListQueryByFd(struct usrNode *phead, int fd){
	while(phead){
		if(phead->sfd == fd)
			return phead;
		phead = phead->next;
	}
	return NULL;
}
int usrListDeleteByFd(struct usrNode **pphead, int fd){
	struct usrNode *pPast, *pCur;
	pPast = pCur = *pphead;
	while( pCur && pCur->sfd!=fd ){
		pPast = pCur;
		pCur = pCur->next;
	}
	if(NULL==pCur){
		return -1;
	}
	if(pPast==pCur){
		*pphead = pCur->next;
	}else{
		pPast->next = pCur->next;
	}
	free(pCur);
	return 0;
}
