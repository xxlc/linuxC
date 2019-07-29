#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(){
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in myAddr;
	bzero(&myAddr, sizeof(myAddr));
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(2000);
	myAddr.sin_addr.s_addr = INADDR_ANY;
	int reuse = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int));
	bind(sfd, (struct sockaddr*)&myAddr, sizeof(struct sockaddr));
	listen(sfd, 10);
	
	int newfd = accept(sfd, NULL, NULL);
	char buf[1024];
	fd_set readsets;
	fd_set MonitorSets;
	FD_ZERO(&MonitorSets);
	FD_SET(STDIN_FILENO, &MonitorSets);
	FD_SET(sfd, &MonitorSets);
	FD_SET(newfd, &MonitorSets);
	int ret;
	while(1){
	    	readsets = MonitorSets;	
		select(newfd+1, &readsets, NULL, NULL, NULL);
		if(FD_ISSET(STDIN_FILENO, &readsets)){
			bzero(buf, sizeof(buf));
			read(STDIN_FILENO, buf, sizeof(buf));
			ret = send(newfd, buf, strlen(buf)-1, 0);
			if( -1==ret ){
				perror("send");
				return -1;
			}
		}
		if(FD_ISSET(sfd, &readsets)){
			newfd = accept(sfd, NULL, NULL);
			FD_SET(newfd, &MonitorSets);
		}
		if(FD_ISSET(newfd, &readsets)){
			bzero(buf, sizeof(buf));
			ret = recv(newfd, buf, sizeof(buf)-1, 0);
			if( 0==ret ){
				printf("byebye.\n");
			    	FD_CLR(newfd, &MonitorSets);
				close(newfd);
			}
			printf("%s\n", buf);
		}
	}
	close(newfd);
	close(sfd);
	return 0;	
}
