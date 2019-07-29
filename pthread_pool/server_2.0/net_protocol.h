#ifndef __NET_PROTOCOL_H__
#define __NET_PROTOCOL_H__

#include "head.h"
#include "task_que.h"

//int sendFileFunc(void*, int);	    client   getsFile
//int recvFileFunc(void*, int);     client   putsFile
//int 
//int 
typedef enum{
	COMMAND_CD=0,	//
	COMMAND_LS,
	COMMAND_PUTS,
	COMMMAND_GETS,
	COMMAND_REMOVE,
	COMMAND_PWD
}Command_e;

/*
功能：通过预设应用协议分析出正确的请求任务
      并将任务参数和相应处理函数放入pTask中
参数：struct tag_task *pTask, const void* packet, size_t packetLen
*/
int parsePacket(struct tag_task *pTask, const void*, size_t);


#endif
