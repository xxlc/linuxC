#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void clean(void* arg){
    	free(arg);
	printf("clean success.\n");
}

void* pthread_start(void *p){
	char *buf = (char*)malloc(128);
	printf("malloc success.\n");
	pthread_cleanup_push(clean, (void*)buf);
	pthread_cleanup_pop(1);
	
	read(STDIN_FILENO, buf, sizeof(buf)-1);
	pthread_exit(NULL);
}

int main(){
	pthread_t pthid;
	pthread_create(&pthid, NULL, pthread_start, NULL);
	
	sleep(2);
	pthread_cancel(pthid);
	return 0;
}
