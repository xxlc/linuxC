#include "epoll_op.h"

int epollAdd(int epfd, int fd){
	int ret;
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
	if(-1==ret){
		perror("epoll_ctl ADD");
		return -1;
	}
	return 0;
}

int epollDel(int epfd, int fd){
	int ret;
	struct epoll_event event;
	ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);
	if(-1==ret){
		perror("epoll_ctl DEL");
		return -1;
	}
	return 0;
}
