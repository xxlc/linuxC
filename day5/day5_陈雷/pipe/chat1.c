#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <time.h>

void print(char *buf){
	time_t cur = time(NULL);
	char *curTime = ctime(&cur);
	printf("usr2 %s%s\n", curTime, buf);
}

int main(int argc, char* argv[]){
	int fdr = open(argv[1], O_RDONLY);
	int fdw = open(argv[2], O_WRONLY);
	printf("I'm chat1.\n");
	
	fd_set readset;
	
	int ret;
	char buf[1024];
	while(1){
		FD_ZERO(&readset);
		FD_SET(fdr, &readset);
		FD_SET(STDOUT_FILENO, &readset);
		ret = select(fdw+1, &readset, NULL, NULL, NULL);
		if( FD_ISSET(fdr, &readset) ){
			bzero(buf, sizeof(buf));
			ret = read(fdr, buf, sizeof(buf));
			if( 0==ret ){
				print("byebye.");
				break;
			}
			print(buf);	
		}	
		if( FD_ISSET(STDOUT_FILENO, &readset) ){
			bzero(buf, sizeof(buf));
			read(STDOUT_FILENO, buf, sizeof(buf));
			write(fdw, buf, strlen(buf)-1);
		}
	}
	close(fdr);
	close(fdw);
	return 0;
}
