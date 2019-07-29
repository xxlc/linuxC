#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char* argv[]){
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if( -1==sfd ){
		printf("socket failed.\n");
		return -1;
	}

	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(8888);//sin_port置0，函数会自动选择一个未占用的端口
	my_addr.sin_addr.s_addr = inet_addr("192.168.5.163");//INADDR_ANY,本机IP
	//将socket文件与相应的IP地址和端口绑定
	int ret = bind(sfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
	if( -1==ret ){
		perror("bind");
		return -1;
	}

	ret = listen(sfd, 10);//监听
	if( -1==ret ){
		perror("listen");
		return -1;
	}
	
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	int new_fd;
	//while(1){
		bzero(&clientaddr, sizeof(clientaddr));
		addrlen = sizeof(struct sockaddr);
		new_fd = accept(sfd, (struct sockaddr*)&clientaddr, &addrlen); 
		if( -1==new_fd ){
			perror("accept");
			return -1;
		}
		send(new_fd, "Hi, wo hen shuai.\n", 17, 0);
		char buf[1024]={0};
		recv(new_fd, buf, sizeof(buf), 0);
		printf("%s\n", buf);
	//}
    	return 0;
}
