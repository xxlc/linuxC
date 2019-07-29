#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
int main(int argc, char* argv[]){
	errno = atoi(argv[1]);
	printf("errno = %d", errno);
	perror("Test");
	
	return 0;
}
