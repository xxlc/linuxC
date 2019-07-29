#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int main(){
	sigset_t set;
	sigemptyset(&set);//清空信号集合
	sigaddset(&set, SIGINT);//往信号集合中添加SIGINT信号
	
	sigprocmask(SIG_BLOCK, &set, NULL);
	sleep(5);
	sigset_t sig;
	sigemptyset(&sig);
	sigpending(&sig);
	if( sigismember(&sig, SIGINT) ){
		printf("SIGINT is exist.\n");
	}else{
		printf("SIGINT is not exist.\n");
	}

	sigprocmask(SIG_UNBLOCK, &set, NULL);
	return 0;
}
