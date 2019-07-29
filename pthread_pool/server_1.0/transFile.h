#ifndef __TRANSFILE_H__
#define __TRANSFILE_H__

#include "head.h"

struct file_node{
	int clientFd;
	char fileName[50];
};

int sendFileByMmap(int targetFd, char *fileName);
int recvCycle(int sfd, char *addr, size_t size);
int recvFileByMmap(int targetFd);

int handleTransFile(void*);

#endif
