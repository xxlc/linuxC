#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
	int fd = open("file", O_RDWR);
	write(fd, "hello", 5);
	return 0;
}
