#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}Data_t, *pData_t;

void clean(void *p){
	pthread_mutex_unlock( &((pData_t)p)->mutex );
	printf("mutext unlock success.\n");
}

void* pthread_start(void *p){
	pData_t pArgs = (pData_t)p;

	pthread_mutex_lock(&pArgs->mutex);
	pthread_cleanup_push(clean, (void*)pArgs);
	pthread_cond_wait(&pArgs->cond, &pArgs->mutex);
	printf("I am wake up.\n");
	pthread_cleanup_pop(1);

    	pthread_exit(NULL);
}

int main(){
	Data_t data;
	pthread_t pthid;
	pthread_mutex_init(&data.mutex, NULL);
	pthread_cond_init(&data.cond, NULL);

	pthread_create(&pthid, NULL, pthread_start, (void*)&data);
	getchar();
	//pthread_cond_signal(&data.cond);
	pthread_cancel(pthid);
	pthread_join(pthid, NULL);
	return 0;
}
