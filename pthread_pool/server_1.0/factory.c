#include "factory.h"

int factoryInit(pFactory_t pFac,int taskLen,int threadNum,int (*taskHandleFunc)(void*)){
	queInit(&pFac->que, taskLen);
	pthread_cond_init(&pFac->cond, NULL);
	pFac->pthid = (pthread_t*)calloc(threadNum, sizeof(pthread_t));
	pFac->threadNum = threadNum;
	pFac->taskHandleFunc = taskHandleFunc;
	pFac->startFlag = 0;
	return 0;
}

int factoryStart(pFactory_t pFac, char *facName){
	pthread_t *pthid = pFac->pthid;
	int threadNum = pFac->threadNum;
	for(int i=0; i<threadNum; i++){
		pthread_create(pthid+i, NULL, threadWorker, (void*)pFac);
	}
	printf("%s start success\n", facName);
	pFac->startFlag = 1;
	return 0;
}
