#include "task_que.h"

//初始化队列头尾指针，队列最大长度，队列当前长度，初始化队列锁
void queInit(pQue_t pQue, int queCapacity){
	pQue->queHead = pQue->queTail = NULL;
	pQue->queSize = 0;
	pQue->queCapacity = queCapacity;
	pthread_mutex_init(&pQue->mutex, NULL);	
}

//入队，从队列尾插入任务结点，传入任务结点空间的地址
int enQueue(pQue_t pQue, pNode_t pNode){
	if(pQue->queSize >= pQue->queCapacity){
		return -1;
	}
	if(NULL==pQue->queHead){
		pQue->queHead = pQue->queTail = pNode;
	}else{
		pQue->queTail->pNext = pNode;
		pQue->queTail = pNode;
	}
	pQue->queSize++;
	return 0;
}

//出队
int deQueue(pQue_t pQue, pNode_t *ppNode){
	if(0==pQue->queSize){
		return -1;
	}
	*ppNode = pQue->queHead;
	pQue->queHead = pQue->queHead->pNext;
	(*ppNode)->pNext = NULL;
	pQue->queSize--;
	return 0;
}
