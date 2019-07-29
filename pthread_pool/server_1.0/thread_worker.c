#include "thread_worker.h"

static void clean(void *pMutex){
	pthread_mutex_unlock((pthread_mutex_t*)pMutex);
}

void* threadWorker(void *args){
	pFactory_t fac = (pFactory_t)args;
	int (*taskHandleFunc)(void*);			//声明任务处理函数
	taskHandleFunc = fac->taskHandleFunc;	//取得到任务处理函数指针
	pQue_t pq = &fac->que;					//任务队列
	pthread_cond_t *pCond = &fac->cond;		//条件变量
	pthread_mutex_t *pMutex = &pq->mutex;	//队列锁
	pNode_t pNode;
	int ret;
	while(1){
		pthread_mutex_lock(pMutex);
		pthread_cleanup_push(clean, pMutex);
		if(0==pq->queSize){
			printf("I am threadWorker.I am wait\n");
			pthread_cond_wait(pCond, pMutex);
		}
		ret = deQueue(pq,&pNode);
		pthread_cleanup_pop(1);
		if(-1 == ret)
			continue;
		printf("I am threadWorker. start working\n");
		//task start
		taskHandleFunc(pNode->pTask);
		free(pNode->pTask);
		free(pNode);
		//task end
	}
	pthread_exit(NULL);
}
