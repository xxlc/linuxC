#ifndef __SERVER_FUNCTION_H__
#define __SERVER_FUNCTION_H__

#include "head.h"
#include "user.h"
#include "task_que.h"

#define COMMAND_CD      0
#define COMMAND_LS      1
#define COMMAND_PUTS    2
#define COMMAND_GETS  	3
#define COMMAND_REMOVE  4
#define COMMAND_PWD     5

/*
   功能：通过预设应用协议分析出正确的请求任务
   并将任务参数和相应处理函数放入pTask中
   参数：struct tag_task *pTask, void* packet
*/
int parsePacket(struct tag_task *pTask, void* packet);

//cd
int handleCd(void *args, pUser_t pUser);
//ls
int handleLs(void *args, pUser_t pUser);
//puts
int handlePuts(void *args, pUser_t pUser);
//gets
int handleGets(void *args, pUser_t pUser);
//remove
int handleRemove(void *args, pUser_t pUser);
//pwd
int handlePwd(void *args, pUser_t pUser);

#endif
