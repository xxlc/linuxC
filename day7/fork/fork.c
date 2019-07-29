#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
int main(){
    	int status;
	pid_t pid;
	if( !fork() ){
		printf("I am child. my pid=%d\n", getpid());
		return 0;
	}else{
		printf("I am parent. my pid=%d\n", getpid());
		sleep(1);
		pid = wait( &status );
		if( WIFEXITED(status) ){
			printf("childPid=%d, return 0", pid);
		}else{
			printf("childPid=%d, return %d", pid, WEXITSTATUS(status));
			return 0;
		}
	}
}
