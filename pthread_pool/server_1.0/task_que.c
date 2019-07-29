#include "task_que.h"

void queInit(pQue_t pq, int queMax){
	pq->queMax = queMax;
	pq->queHead = pq->queTail = NULL;
	pq->queSize = 0;
	pthread_mutex_init(&pq->mutex, NULL);
}

int enQueue(pQue_t pq, pNode_t pNode){
	if(NULL==pq){
		printf("enQueue failed. the pointer of queue is NULL\n");
		return -1;
	}
	if(pq->queSize==pq->queMax){
		printf("enQueue failed. queue is FULL\n");
		return -1;
	}
	if(pq->queHead==NULL){
		pq->queHead = pq->queTail = pNode;
	}else{
		pq->queTail->pNext = pNode;
		pq->queTail = pNode;
	}
	pq->queSize++;
	return 0;
}

int deQueue(pQue_t pq, pNode_t *ppNode){
	if(NULL==pq){
		printf("deQueue failed. the pointer of queue is NULL\n");
		return -1;
	}
	if(0==pq->queSize){
		printf("deQueue failed. queue is empty\n");
		return -1;
	}
	*ppNode = pq->queHead;
	pq->queHead = (*ppNode)->pNext;
	(*ppNode)->pNext = NULL;
	pq->queSize--;
	return 0;
}
