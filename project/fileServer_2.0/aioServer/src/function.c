#include "../include/function.h"

static void clean(void* args){ 
	pthread_mutex_unlock((pthread_mutex_t*)args);
}
void* pthreadFunc(void* args){
	pFactory_t pFactory = (pFactory_t)args;
	pthread_cond_t *pCond = &pFactory->cond;
	pQue_t pq = &pFactory->que;
	pthread_mutex_t *pMutex = &pq->mutex;
	pNode_t pTaskNode;
	int ret;
	while(1){
		pthread_mutex_lock(pMutex);
		pthread_cleanup_push(clean, pMutex);
		if(0==pq->queSize){
			pthread_cond_wait(pCond, pMutex);
		}   
		ret = deQueue(pq, &pTaskNode);
		pthread_cleanup_pop(1);                                                                 
		if(-1==ret){
			printf("deQueue failed\n");
			continue;
		}   
		//任务开始
		printf("I am thread. I am working.\n"); 
		/*  to do something

		 */
		printf("I am thread. work success.\n"); 
		epollAdd(pFactory->epfd, pTaskNode->clientFd);
		//任务结束
	}   
	pthread_exit(NULL);
}
