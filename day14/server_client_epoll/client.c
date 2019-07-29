#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>

int main(){
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(2000);
	serverAddr.sin_addr.s_addr = inet_addr("192.168.5.163");
	connect(sfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));

	char buf[1024];
	fd_set readsets;
	int ret;
	while(1){
	    	FD_ZERO(&readsets);
		FD_SET(STDIN_FILENO, &readsets);
		FD_SET(sfd, &readsets);
		select(sfd+1, &readsets, NULL, NULL, NULL);
		if(FD_ISSET(STDIN_FILENO, &readsets)){
			bzero(buf, sizeof(buf));
			read(STDIN_FILENO, buf, sizeof(buf));
			send(sfd, buf, strlen(buf)-1, 0);
		}
		if(FD_ISSET(sfd, &readsets)){
			bzero(buf, sizeof(buf));
			ret = recv(sfd, buf, sizeof(buf)-1, 0);
			if( 0==ret ){
				printf("byebye.\n");
				return 0;
			}
			printf("%s\n", buf);
		}
	}
	return 0;	
}
