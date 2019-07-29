#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
	//creat a file in hugetlb fs
    	int fd = open("/home/chenlei/Documents/day8/hugepage/file", O_CREAT|O_RDWR, 0666);
	if(fd<0){
		perror("open");
		return -1;
	}

	//map the file into address space of current application process
	char *addr = (char*)mmap(0, 1<<21, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(MAP_FAILED==addr){
		perror("mmap");
		close(fd);
		return -1;
	}

	//from now on, you can store application data on huge pages via addr
	strcpy(addr, "Hello");

	munmap(addr, 1<<21);
	close(fd);
	return 0;
}
