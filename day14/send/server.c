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
	printf("%d\n", newfd);
	while(1);
	close(newfd);
	close(sfd);
	return 0;	
}
