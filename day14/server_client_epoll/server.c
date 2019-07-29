#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>

int main(){
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	int reuse = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int));
	struct sockaddr_in myAddr;
	bzero(&myAddr, sizeof(myAddr));
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(2000);
	myAddr.sin_addr.s_addr = INADDR_ANY;
	bind(sfd, (struct sockaddr*)&myAddr, sizeof(struct sockaddr));
	listen(sfd, 10);
	
	int newfd = accept(sfd, NULL, NULL);
	int epfd = epoll_create(1);
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = sfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
	event.data.fd = newfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &event);
	event.data.fd = STDIN_FILENO;
	epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
	int max = 64;
	struct epoll_event *events;
       	events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*max);
	char buf[1024];
	int ret, n, i;
	while(1){
	    	n = epoll_wait(epfd, events, max, -1);
		for(i=0; i<n; i++){
	    		if(events[i].data.fd==STDIN_FILENO){
				bzero(buf, sizeof(buf));
				read(STDIN_FILENO, buf, sizeof(buf));
				ret = send(newfd, buf, strlen(buf)-1, 0);
				if( -1==ret ){
					perror("send");
					return -1;
				}
			}
			if(events[i].data.fd==sfd){
				newfd = accept(sfd, NULL, NULL);
				event.events = EPOLLIN;
				event.data.fd = newfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &event);
			}	
			if(events[i].data.fd==newfd){
				bzero(buf, sizeof(buf));
				ret = recv(newfd, buf, sizeof(buf)-1, 0);
				if( 0==ret ){
					printf("byebye.\n");
					epoll_ctl(epfd, EPOLL_CTL_DEL, newfd, &event);
					close(newfd);
				}
				printf("%s\n", buf);
			}
		}
	}
	close(newfd);
	close(sfd);
	return 0;	
}
