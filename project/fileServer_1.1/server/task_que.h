#ifndef __TASK_QUE_H__
#define __TASK_QUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "user.h"

//任务结点，结构体内定义任务指针
struct tag_task{
	void *args;
	int (*handle)(void*, pUser_t);
};

typedef struct tag_node{
	struct tag_task task;
	pUser_t pUser;
	int epfd;
	struct tag_node *pNext;
}Node_t, *pNode_t;

typedef struct{
	pNode_t queHead, queTail;
	int queMax;
	int queSize;
	pthread_mutex_t mutex;
}Que_t, *pQue_t;

void queInit(pQue_t pq, int queMax);

int enQueue(pQue_t pq, pNode_t pNode);

int deQueue(pQue_t pq, pNode_t *ppNode);


#endif
