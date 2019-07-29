#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in myAddr;
	bzero(&myAddr, sizeof(myAddr));
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = 0;
	myAddr.sin_addr.s_addr = INADDR_ANY;
	bind(sfd, (struct sockaddr*)&myAddr, sizeof(struct sockaddr));
	
	struct sockaddr_in serAddr;
	bzero(&serAddr, sizeof(serAddr));
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
	serAddr.sin_addr.s_addr = inet_addr("192.168.5.163");

	connect(sfd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
	char buf[1024]={0};
	recv(sfd, buf, sizeof(buf), 0);
	printf("%s\n", buf);
	bzero(buf, sizeof(buf));
	strcpy(buf, "heihei, wobinishuai.\n");
	send(sfd, buf, strlen(buf), 0);
	sleep(10);
	close(sfd);
    	return 0;
}
