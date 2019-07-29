#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	printf("curPid=%d, parentPid=%d\n", getpid(), getppid());
	printf("uid=%d, gid=%d\n", getuid(), getgid());
	printf("euid=%d, egid=%d\n", geteuid(), getegid());
	return 0;
}
