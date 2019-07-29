#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
    	int fd = open("./file", O_RDWR);
	printf("fd=%d\n", fd);

	struct stat buf;
	stat("./file", &buf);
	long page_size = sysconf(_SC_PAGESIZE);
	printf("filesize=%ld, pagesize=%ld\n", buf.st_size, page_size);
	char *ptr = (char*)mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if( (char*)-1==ptr ){
		perror("mmap\n");
		return -1;
	}
	strcpy(ptr, "world");
	munmap(ptr, buf.st_size);

	close(fd);
	return 0;
}
