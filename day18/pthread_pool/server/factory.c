#include "factory.h"
#include "task_que.h"
#include "transFile.h"
#include "epoll_op.h"

//传入参数：工厂指针，线程数目，最大任务数
int factoryInit(pFactory_t pFactory, int threadNum, int taskMax){
	queInit(&pFactory->que, taskMax);
	pthread_cond_init(&pFactory->cond, NULL);
	pFactory->epfd = epoll_create(1);
	pFactory->pthid = (pthread_t*)calloc(threadNum, sizeof(pthread_t));
	pFactory->threadNum = threadNum;
	pFactory->startFlag = 0;
	printf("factoryInit success\n");
	return 0;
}


void clean(void* args){	
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
		/*	to do something
		 
		 */
		printf("I am thread. work success.\n");	
		epollAdd(pFactory->epfd, pTaskNode->clientFd);
		//任务结束
	}
	pthread_exit(NULL);
}

//创建各子线程，将各个线程加入cond条件变量队列
int factoryStart(pFactory_t pFactory){
	for(int i=0; i<pFactory->threadNum; i++){
		pthread_create(pFactory->pthid+i, NULL, pthreadFunc, pFactory);
	}
	printf("factoryStart success\n");
	return 0;
} 
