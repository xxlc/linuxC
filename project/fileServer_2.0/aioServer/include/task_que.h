#ifndef __TASK_QUE_H__
#define __TASK_QUE_H__

#include "head.h"

//
typedef struct tag_node{
	int clientFd;
	struct tag_node *pNext;
}Node_t, *pNode_t;

typedef struct{
	pNode_t queHead, queTail;
	int queCapacity;//队列最大长度
	int queSize;//队列当前长度
	pthread_mutex_t mutex;//队列锁，各线程都要访问任务队列，故要控制访问冲突
}Que_t, *pQue_t;

//初始化队列头尾指针，队列最大长度，队列当前长度，初始化队列锁
void queInit(pQue_t, int);

//入队，从队列尾插入任务结点，传入任务结点空间的地址
int enQueue(pQue_t, pNode_t);

//出队
int deQueue(pQue_t, pNode_t *);

#endif
