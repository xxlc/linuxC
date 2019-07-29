#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void sigFunc(int signum, siginfo_t *pSigInfo, void *p){
	printf("pid=%d, uid=%d.\n", pSigInfo->si_pid, pSigInfo->si_uid);
}

int main(){
	struct sigaction act;
	act.sa_flags = SA_SIGINFO|SA_NODEFER;
	act.sa_sigaction = sigFunc;
       	sigaction(SIGINT, &act, NULL);//注册SIGINT信号处理函数
	sigaction(SIGQUIT, &act, NULL);//注册SIGQUIT信号处理函数
	while(1);
	return 0;	
}
