#include "config.h"
#define CURRENT 2
#define ME 1
#define OTHER 2
#define SIZE_2M 2<<21
int clientTcpInit(int *psfd, char *serverIp, unsigned short serverPort);

int main(int argc, char **argv){
	int sfd, ret;
	char serverip[] = "192.168.5.163";
	//与server建立连接
	ret = clientTcpInit(&sfd, serverip, 2000);
	if(-1==ret){
		return -1;
	}	
	//打开一个共享内存，读取消息记录，size大小要与client_show相等
	int shmid = shmget(1000, SIZE_2M, IPC_CREAT|0600);
	char *ptr = (char*)shmat(shmid, NULL, 0);
	//打开一个消息队列
	int msqid = msgget(1000, IPC_CREAT|0600);
	if(-1==msqid){
		perror("msgget");
		return -1;
	}
	//第一条消息为确认用户名
	char name[10]={0};
	recv(sfd, name, sizeof(name), 0);
	//epoll
	int epfd = epoll_create(1);
	struct epoll_event event, events[2];
	event.events = EPOLLIN;
	event.data.fd = STDIN_FILENO;
	epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
	event.data.fd = sfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);	
	struct msgQueueNode message;
	char buf[1024];
	int i, n;
	system("clear");//运行后清屏
	while(1){
	    	n = epoll_wait(epfd, events, 2, -1);
		for(i=0; i<n; i++){
			if(events[i].data.fd==STDIN_FILENO){
			    //发送聊天消息，'\n'也发过去
				bzero(buf, sizeof(buf));
				read(STDIN_FILENO, buf, sizeof(buf)-1);
				send(sfd, buf, strlen(buf), 0);
				system("clear");
			}
			if(events[i].data.fd==sfd){
				recv(sfd, &message, sizeof(message), 0);
				if(message.mtype==CURRENT){
					//这是新消息，收到后做标记加入消息队列
					if( strcmp(name, message.msg.usrname)==0 ){
						message.mtype = ME;//代表自己发出的消息
					}else{
						message.mtype = OTHER;//代表别人发出的消息
					}
					ret = msgsnd(msqid, &message, sizeof(struct msgNode), 0);
					if(-1==ret){
						perror("msgsnd");
						return -1;
					}
				}else{
					//这是聊天记录，收到后放入共享内存
				}
			}	
		}
	}
}

int clientTcpInit(int *psfd, char *serverIp, unsigned short serverPort){
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
	ret = connect(*psfd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
	if(-1==ret){
		perror("connect");
		return -1;
	}
	return 0;
}
