#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
	int fd_log1 = open(argv[1], O_WRONLY|O_APPEND);
	int fd_log2 = open(argv[2], O_WRONLY|O_APPEND);

	dup2(fd_log1, STDOUT_FILENO);
	dup2(fd_log2, STDERR_FILENO);

	printf("I'm log1.\n");
	fprintf(stderr, "I'm log2.\n");
	return 0;
}
