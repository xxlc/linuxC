#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>

void Daemon(){
	if(fork()){
		exit(0);
	}
	setsid();
	chdir("/");
	umask(0);
	for(int i=0; i<3; i++){
		close(i);
	}
}

int main(){
	Daemon();
	int fd = open("/tmp/log", O_WRONLY|O_CREAT, 0666);
	while(1){
		write(fd, "wohenshuai\n", 11);
		fsync(fd);
		sleep(60);
	}
	return 0;
}
