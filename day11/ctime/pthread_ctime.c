#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void* pthread_start(void *args){
	time_t now;
	time(&now);
	char* p = ctime(&now);
	printf("I am child thread, p = %s\n", p);
	sleep(4);
	printf("I am child thread, p = %s\n", p);
	
    	pthread_exit(NULL);
}

int main(){
	pthread_t pthid;
	pthread_create(&pthid, NULL, pthread_start, NULL);

	time_t now;
	sleep(2);
	time(&now);
	char* p = ctime(&now);
	printf("I am main thread, p = %s\n", p);
	pthread_join(pthid, NULL);
	return 0;
}
