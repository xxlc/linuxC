#include "tcp_init.h"

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
	int reuse = 1;
	ret=setsockopt(*psfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int));
	if(-1==ret){
		perror("setsockopt");
		return -1;
	}
	ret = bind(*psfd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
	if(-1==ret){
		perror("bind");
		return -1; 
	}   
	listen(*psfd, LISTEN_MAX);
	printf("serverTcpInit success\n");
	return 0;
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
		perror("clientTcpInit");
		return -1;
	}
	printf("clientTcpInit success\n");
	return 0;
}
