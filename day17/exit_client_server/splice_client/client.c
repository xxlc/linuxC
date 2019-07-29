#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>

int recvCycle(int sfd, char *addr, size_t size){
	size_t total=0, ret;
	while(total<size){
		ret = recv(sfd, addr+total, size-total, 0);
		if(0==ret){
			printf("recvFileByMmap failed. the return of recv is zero\n");
			return -1;
		}
		total += ret;
	}
	return 0;
}
int recvFileByMmap(int targetFd){
	char fileName[100] = {0};
	int dataLen;
	size_t fileSize;
	//文件名
	recvCycle(targetFd, (char*)&dataLen, 4); 
	recvCycle(targetFd, fileName, dataLen);
	//文件大小
	recvCycle(targetFd, (char*)&dataLen, 4); 
	recvCycle(targetFd, (char*)&fileSize, dataLen);
	//通过splice接收整个文件
	int fds[2];
	pipe(fds);
	int fd = open(fileName, O_CREAT|O_RDWR, 0644);
	ftruncate(fd, fileSize);
	//开始接收文件
	printf("start receive %s total=%lu\n", fileName, fileSize);
	time_t startTime, endTime;
	time(&startTime);
	size_t block, lastLoadSize, downLoadSize, ret;
	block = fileSize/100;
	downLoadSize = lastLoadSize = 0;
	while(downLoadSize<fileSize){
		if(downLoadSize-lastLoadSize>block){
			lastLoadSize = downLoadSize;
			printf("%5.2f%c\r", (float)downLoadSize/fileSize*100, '%');
			fflush(stdout);
		}
		ret=splice(targetFd, NULL, fds[1], NULL, fileSize, SPLICE_F_MOVE|SPLICE_F_MORE);
		splice(fds[0], NULL, fd, NULL, ret, SPLICE_F_MOVE|SPLICE_F_MORE);
		if(ret==0){
			printf("recvFileByMmap failed. the return of recv is zero\n");
			close(fd);
			return -1;
		}
		downLoadSize += ret;
	}
	time(&endTime);
	close(fd);
	printf("recv %s success, spend time %ld s\n", fileName, endTime-startTime);
	return 0;
} 

int main(int argc, char* argv[]){
	if(argc!=3){
		printf("argc!=3\n");
		return -1;
	}
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((unsigned short)atoi(argv[2]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
	connect(sfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));

	int epfd = epoll_create(1);
	struct epoll_event event, *events;
	events = (struct epoll_event*)calloc(1, sizeof(struct epoll_event));
	event.events = EPOLLIN;
	event.data.fd = sfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);

	epoll_wait(epfd, events,1, -1);
	int ret = recvFileByMmap(events[0].data.fd);
	if(-1==ret){
		printf("recvFileByMmap failed\n");
		return -1;
	}
	return 0;	
}
