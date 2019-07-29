#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void clean1(void *args){
	printf("I am clean1.\n");
}
void clean2(void *args){
	printf("I am clean2.\n");
}
void* pthread_start(void *args){
	char buf[128];
    	pthread_cleanup_push(clean1, NULL);
	pthread_cleanup_push(clean2, NULL);
	read(STDIN_FILENO, buf, sizeof(buf));
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);

    	pthread_exit(NULL);
}

int main(){
	pthread_t pthid;
	pthread_create(&pthid, NULL, pthread_start, NULL);
	
	sleep(2);
	pthread_cancel(pthid);
	pthread_join(pthid, NULL);
	return 0;
}
