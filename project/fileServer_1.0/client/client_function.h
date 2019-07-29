#ifndef __CLIENT_FUNCTION_H__
#define __CLIENT_FUNCTION_H__

#include "head.h"
#include "transFile.h"
#include "tcp_init.h"

#define COMMAND_CD      0
#define COMMAND_LS      1
#define COMMAND_PUTS    2
#define COMMAND_GETS    3
#define COMMAND_REMOVE  4
#define COMMAND_PWD     5


struct tag_prompt{
	char usrName[10];
	char curDir[80];
};
struct tag_request{
	size_t packetLen;
	int packetType;
	char packetData[100];
};

int userLogin(int *psfd, char *ip, unsigned short port, char *pName);
int userRegister(int *psfd, char *ip, unsigned short port, char *pName);
int commandScanf(struct tag_request *);//读取命令输入

int requestCd(int serverFd, struct tag_request *pReq, char *curDir);
int requestLs();
int requestPuts();
int requestGets();
int requestRemove();
int requestPwd();


#endif
