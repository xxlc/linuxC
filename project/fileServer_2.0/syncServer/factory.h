#ifndef __FACTORY_H__
#define __FACTORY_H__

#include "head.h"
#include "task_que.h"
#include "epoll_op.h"
#include "function.h"

typedef struct{
	Que_t que;
	int epfd;
	pthread_cond_t cond;
	pthread_t *pthid;//存储线程ID的起始地址
	int threadNum;
	unsigned int dirCode[1024];
	int fileDes[1024];
	short startFlag;
}Factory_t, *pFactory_t;

//传入参数：工厂指针，线程数目，最大任务数
int factoryInit(pFactory_t, int, int);

//创建各子线程，将各个线程加入cond条件变量队列
int factoryStart(pFactory_t);


#endif
