#include "../include/factory.h"

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

//创建各子线程，将各个线程加入cond条件变量队列
int factoryStart(pFactory_t pFactory){
	for(int i=0; i<pFactory->threadNum; i++){
		pthread_create(pFactory->pthid+i, NULL, pthreadFunc, pFactory);
	}
	printf("factoryStart success\n");
	return 0;
} 
