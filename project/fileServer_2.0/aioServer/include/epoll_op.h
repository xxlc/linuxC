#ifndef __EPOLL_OP_H__
#define __EPOLL_OP_H__

#include "head.h"

//int epfd, int fd
int epollAdd(int, int);

//int epfd, int fd
int epollDel(int, int);


#endif
