#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define N 20000000
typedef struct{
    	int num;
	pthread_mutex_t mutex;
}Data_t;

Data_t data;

void* pthread_start(void *args){
	for(int i=0; i<N; i++){
		pthread_mutex_lock(&data.mutex);
		data.num++;
		pthread_mutex_unlock(&data.mutex);
	}
    	pthread_exit(NULL);
}

int main(){
	pthread_t pthid;
	pthread_mutex_init(&data.mutex, NULL);
	data.num = 0;
	pthread_create(&pthid, NULL, pthread_start, NULL);
	for(int i=0; i<N; i++){
		pthread_mutex_lock(&data.mutex);
		data.num++;
		pthread_mutex_unlock(&data.mutex);
	}
	pthread_join(pthid, NULL);
	printf("%d\n", data.num);
	return 0;
}
