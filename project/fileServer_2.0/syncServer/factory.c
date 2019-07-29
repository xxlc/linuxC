#include "factory.h"

//传入参数：工厂指针，线程数目，最大任务数
int factoryInit(pFactory_t pFactory, int threadNum, int taskMax){
	queInit(&pFactory->que, taskMax);
	pthread_cond_init(&pFactory->cond, NULL);
	pFactory->epfd = epoll_create(1);
	pFactory->pthid = (pthread_t*)calloc(threadNum, sizeof(pthread_t));
	pFactory->threadNum = threadNum;
	bzero(pFactory->dirCode,sizeof(pFactory->dirCode));
	bzero(pFactory->fileDes,sizeof(pFactory->fileDes));
	pFactory->startFlag = 0;
	printf("factoryInit success\n");
	return 0;
}

//创建各子线程，将各个线程加入cond条件变量队列
int factoryStart(pFactory_t pFactory){
	//由于连接数据库不是线程安全的，所以在连接前先初始化
	//对应的两个接口是：mysql_library_init(0,NULL,NULL);mysql_library_end();
	mysql_library_init(0, NULL, NULL);
	for(int i=0; i<pFactory->threadNum; i++){	
		pthread_create(pFactory->pthid+i, NULL, pthreadFunc, pFactory);
	}
	printf("factoryStart success\n");
	return 0;
} 
