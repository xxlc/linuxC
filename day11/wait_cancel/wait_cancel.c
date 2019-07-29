#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct{
    	int num;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}Data_t;

void clean(void *args){
	Data_t* data = (Data_t*)args;
	pthread_mutex_unlock(&data->mutex);
	printf("clean success.\n");
}
void* pthread_start(void *args){
	Data_t* data = (Data_t*)args;

	pthread_mutex_lock(&data->mutex);
	pthread_cleanup_push(clean, (void*)data);
	pthread_cond_wait(&data->cond, &data->mutex);
	printf("I am wake up.\n");
	pthread_cleanup_pop(1);

    	pthread_exit(NULL);
}
int main(){
	pthread_t pthid1, pthid2;
	Data_t data;
	//对cond和mutex初始化
	pthread_mutex_init(&data.mutex, NULL);
	pthread_cond_init(&data.cond, NULL);
	//创建子线程
	pthread_create(&pthid1, NULL, pthread_start, (void*)&data);
	pthread_create(&pthid2, NULL, pthread_start, (void*)&data);
	
	sleep(2);
	printf("before cancel\n");
	pthread_cancel(pthid1);
	pthread_cancel(pthid2);
	pthread_join(pthid1, NULL);
	pthread_join(pthid2, NULL);
	return 0;
}
