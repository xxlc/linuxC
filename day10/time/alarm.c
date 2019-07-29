#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void sigHandler(int signum){
	printf("signal:%d\n", signum);
}

int main(){
	signal(SIGALRM, sigHandler);
	alarm(3);//告知自身要在三秒后唤醒
	printf("before pause().\n");
	pause();
	printf("after pause().\n");
	return 0;
}
