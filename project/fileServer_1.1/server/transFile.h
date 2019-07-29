#ifndef __TRANSFILE_H__
#define __TRANSFILE_H__

#include "head.h"

int sendFileByMmap(int targetFd, char *fileName, char *rootDir);
int recvCycle(int sfd, char *addr, size_t size);
int recvFileByMmap(int targetFd, char* rootDir, char* curDir);


#endif
