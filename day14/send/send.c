#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <fcntl.h>

int main(){
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(2000);
	serverAddr.sin_addr.s_addr = inet_addr("192.168.5.163");
	
	int flags = fcntl(sfd, F_GETFL, 0);
	fcntl(sfd, F_SETFL,O_NONBLOCK|flags);
	connect(sfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));

	char buf[1000];
	int total=0, ret;
	while(1){
		ret = send(sfd, buf, sizeof(buf), 0);
		if( -1==ret ){
			return 0;
		}
		total += ret;
		printf("ret=%d, total=%d\n", ret, total);
	}
	return 0;	
}
