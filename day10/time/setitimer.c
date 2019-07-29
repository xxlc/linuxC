#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

int real_time = 0;
int virt_time = 0;
int prof_time = 0;

void sigHandler(int signum){
	switch(signum){
		case SIGALRM:	real_time++;break;
		case SIGVTALRM:	virt_time++;break;
		case SIGPROF:	prof_time++;break;
		default:printf("switch error.\n");break;
	}
	printf("real_time=%ds, virt_time=%ds, prof_time=%ds\n",
	       	real_time, virt_time, prof_time);
}

int main(){
	struct itimerval set_t;
	bzero(&set_t, sizeof(struct itimerval));
	set_t.it_value.tv_sec = 1;
	set_t.it_interval.tv_sec = 1;

	signal(SIGALRM, sigHandler);
	signal(SIGVTALRM, sigHandler);
	signal(SIGPROF, sigHandler);

	setitimer(ITIMER_VIRTUAL, &set_t, NULL);
	setitimer(ITIMER_PROF, &set_t, NULL);
	sleep(5);
	setitimer(ITIMER_REAL, &set_t, NULL);
	while(1);
	return 0;
}
