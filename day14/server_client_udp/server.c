#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(){
	int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in myAddr;
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(2000);
	myAddr.sin_addr.s_addr = inet_addr("192.168.5.163");
	bind(sfd, (struct sockaddr*)&myAddr, sizeof(struct sockaddr));

	char buf[1024]={0};
	struct sockaddr_in fromaddr;
	socklen_t fromlen = sizeof(struct sockaddr);
	recvfrom(sfd, buf, sizeof(buf)-1, 0, (struct sockaddr*)&fromaddr, &fromlen);
	printf("receive from %s:%d. %s\n", inet_ntoa(fromaddr.sin_addr),
		ntohs(fromaddr.sin_port), buf);
	fd_set readsets;
	while(1){
	    	FD_ZERO(&readsets);
		FD_SET(STDIN_FILENO, &readsets);
		FD_SET(sfd, &readsets);
		select(sfd+1, &readsets, NULL, NULL, NULL);
		if(FD_ISSET(STDIN_FILENO, &readsets)){
	    		bzero(buf, sizeof(buf));
			read(STDIN_FILENO, buf, sizeof(buf));
			sendto(sfd, buf, strlen(buf)-1, 0, (struct sockaddr*)&fromaddr,
				sizeof(struct sockaddr));
		}
		if(FD_ISSET(sfd, &readsets)){
			bzero(buf, sizeof(buf));
			recvfrom(sfd, buf, sizeof(buf)-1, 0, (struct sockaddr*)&fromaddr,
				&fromlen);
			printf("%s\n", buf);
		}
	}
	return 0;
}
