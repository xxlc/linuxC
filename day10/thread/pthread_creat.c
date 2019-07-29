#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void* pthFunc(void *p){
	printf("I am thread.\n");
	printf("val = %d\n", *(int*)p);
	pthread_exit((void*)2);
}

int main(){
	pthread_t pthid;
	int val = 1;
	int ret = pthread_create(&pthid, NULL, pthFunc, &val);
	if( ret!=0 ){
		printf("pthread_creat:%s\n", strerror(ret));
		return -1;
	}
	long a;
	pthread_join(pthid, (void**)&a);
	printf("pthread return %ld\n", a);
	return 0;
}
