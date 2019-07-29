#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(){
	if( !fork() ){
	    	errno = 0;
	    	int ret = nice(10);
		if( -1==ret && errno!=0 ){
			perror("nice");
			exit(1);
		}
		printf("nice value is now %d.\n", ret);
		while(1);
	}else{
	    	while(1);
		wait(NULL);
		return 0;
	}
}

