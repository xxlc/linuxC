#include <stdio.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(){
	int fd = open("./file", O_RDWR);
	
	struct iovec iov[2];
	char buf1[10] = "hello";
	iov[0].iov_base = buf1;
	iov[0].iov_len = 5;
	char buf2[10] = "world";
	iov[1].iov_base = buf2;
	iov[1].iov_len = 5;
	writev(fd, iov, 2);
	close(fd);
	return 0;
}
