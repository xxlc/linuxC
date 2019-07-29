#ifndef __TCP_INIT_H__
#define __TCP_INIT_H__

#include "head.h"
#define LISTEN_MAX 10

/*
 int *psfd;
 char *serverIp;
 unsigned short serverPort;
 */
int serverTcpInit(int *, char *, unsigned short);
int clientTcpInit(int *, char *, unsigned short);

#endif
