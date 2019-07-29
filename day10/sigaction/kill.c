#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char* argv[]){
	printf("pid=%d, uid=%d", getpid(), getuid());
	kill(atoi(argv[1]), SIGINT);
	
	return 0;
}
