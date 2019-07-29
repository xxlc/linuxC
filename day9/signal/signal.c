#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void sigHandler(int signum){
	printf("befor sleep %d is coming\n", signum);
	sleep(2);
	printf("after sleep %d is coming\n", signum);
}

int main(int argc, char* argv[]){
	if( signal(SIGINT, sigHandler)==SIG_ERR ){
		perror("signal");//注册失败
		return -1;
	}
	if( signal(SIGQUIT, sigHandler)==SIG_ERR ){
		perror("signal");//注册失败
		return -2;
	}
	while(1);
	return 0;
}
