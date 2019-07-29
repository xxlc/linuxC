#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
	int fdw = open(argv[1], O_WRONLY);
	int fdr = open(argv[2], O_RDONLY);
	struct pollfd fds[2];
	fds[0].fd = fdr;
	fds[0].events = POLLIN|POLLPRI;
	fds[1].fd = STDIN_FILENO;
	fds[1].events = POLLIN|POLLPRI;
	char buf[1024];
	int ret;
	printf("I'm chat2\n");
	while(1){
		ret = poll(fds, 2, 3000);
		printf("poll = %d\n", ret);
		if( fds[0].revents&POLLIN ){
		    	memset(buf, 0, sizeof(buf));
			ret = read(fdr, buf, sizeof(buf)-1);
			if( 0==ret )
				return 0;
			printf("%s\n", buf);	
		}
		if( fds[1].revents&POLLIN ){
			memset(buf, 0, sizeof(buf));
			read(STDIN_FILENO, buf, sizeof(buf));
			write(fdw, buf, strlen(buf)-1);
		}
	}
	return 0;
}
