#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define N 20000000

typedef struct{
	int val;
	pthread_mutex_t mutex;
}Val_t;

void* pthread_start(void *p){
	Val_t *data = (Val_t*)p;
	for(int i=0; i<N; i++){
	    	pthread_mutex_lock(&data->mutex);
		data->val++;
	    	pthread_mutex_unlock(&data->mutex);
	}
	pthread_exit(NULL);
}

int main(){
	Val_t data;
	data.val = 0;
	pthread_mutex_init(&data.mutex, NULL);

	pthread_t pthid;
	pthread_create(&pthid, NULL, pthread_start, &data.val);
	for(int i=0; i<N; i++){
		pthread_mutex_lock(&data.mutex);
		data.val++;
		pthread_mutex_unlock(&data.mutex);
	}
	pthread_join(pthid, NULL);
	printf("%d\n", data.val);
	return 0;
}
