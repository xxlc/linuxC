#ifndef __TRANSFILE_H__
#define __TRANSFILE_H__

#include "head.h"

int sendFileByMmap(int targetFd, char *fileName);
int recvCycle(int sfd, char *addr, size_t size);
int recvFileByMmap(int targetFd);


#endif
