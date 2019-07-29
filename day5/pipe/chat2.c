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
	printf("usr1 %s%s\n", curTime, buf);
}

int main(int argc, char* argv[]){
	int fdw = open(argv[1], O_WRONLY);
	int fdr = open(argv[2], O_RDONLY);
	printf("I'm chat2.\n");
	
	fd_set readset;
	int ret;
	char buf[1024];
	while(1){
		FD_ZERO(&readset);
		FD_SET(fdr, &readset);
		FD_SET(STDIN_FILENO, &readset);
		ret = select(fdr+1, &readset, NULL, NULL, NULL);
		if( FD_ISSET(fdr, &readset) ){
			bzero(buf, sizeof(buf));
			ret = read(fdr, buf, sizeof(buf));
			if( 0==ret ){
				print("byebye.");
				break;
			}
			print(buf);	
		}	
		if( FD_ISSET(STDIN_FILENO, &readset) ){
			bzero(buf, sizeof(buf));
			read(STDIN_FILENO, buf, sizeof(buf));
			write(fdw, buf, strlen(buf)-1);
		}
	}
	close(fdr);
	close(fdw);
	return 0;
}
