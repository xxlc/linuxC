#ifndef __FACTORY_H__
#define __FACTORY_H__

#include "head.h"
#include "task_que.h"
#include "thread_worker.h"

typedef struct{
	Que_t que;
	pthread_cond_t cond;
	pthread_t *pthid;	//存储线程ID的起始地址
	int threadNum;		//线程数目
	int (*taskHandleFunc)(void*);
	short startFlag;
}Factory_t, *pFactory_t;
//初始化工厂
int factoryInit(pFactory_t pFac, int taskLen, int threadNum, int (*taskHandleFunc)(void*));
//启动工厂
int factoryStart(pFactory_t pFac, char *facName);


#endif
