#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main(){
	int fds[2];
	pipe(fds);

	if(!fork()){
		close(fds[0]);
		write(fds[1], "Hello", 5);
		sleep(2);
		close(fds[1]);
		exit(0);
	}else{
		close(fds[1]);
		sleep(2);
		char buf[128];
		if( read(fds[0], buf, sizeof(buf)-1)>0 ){
			buf[strlen(buf)] = '\0';
			printf("%s\n", buf);
		}
		waitpid(-1, NULL, 0);
		close(fds[0]);
		return 0;
	}
}
