#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void sigFunc(int signum, siginfo_t *pSigInfo, void *p){
	sigset_t set;

	printf("before sleep(4).signal=%d.\n", signum);
	sleep(4);
	printf("after sleep(4).signal=%d.\n", signum);
	sigpending(&set);
	if( sigismember(&set, SIGQUIT) ){
		printf("%d signal is exist.\n", SIGQUIT);
	}else{
		printf("%d signal is not exist.\n", SIGQUIT);
	}
}

int main(){
	struct sigaction act;
	act.sa_flags = SA_SIGINFO|SA_NODEFER;
	act.sa_sigaction = sigFunc;
       	sigemptyset(&act.sa_mask);//清空掩码信号集合
	sigaddset(&act.sa_mask, SIGQUIT);//将SIGQUIT信号添加到掩码集合
	sigaction(SIGINT, &act, NULL);//注册SIGINT信号处理函i数

	while(1);
	return 0;	
}
