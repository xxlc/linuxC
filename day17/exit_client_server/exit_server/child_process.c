#include "config.h"

int create_child_process(proInfo_t *pInfo, int proNum){
	int fds[2];
	int ret;
	pid_t pid;
	for(int i=0; i<proNum; i++){
		ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
		if(-1==ret){
			perror("socketpair");
			return -1;
		}
		pid = fork();
		if(0==pid){
			close(fds[0]);
			process_work(fds[1]);
		}else{
			close(fds[1]);
			pInfo[i].childFd = fds[0];
			pInfo[i].pid = pid;
			pInfo[i].stats = LEISURE;
		}
	}
	return 0;
}

int process_work(int parentFd){
	int clientFd, exitFlag;
	char fileName[] = "file";
	char finishFlag;
	while(1){
		recvFd(parentFd, &clientFd, &exitFlag);
		if(1==exitFlag){
			printf("%d pid process exit\n", getpid());
			exit(0);
		}
		//拿到clientfd后开始工作
		printf("I am child process. I am working.\n");
		int ret = sendFileByMmap(clientFd, fileName);
		if(-1==ret){
			printf("sendFileByMmap");
		}
		close(clientFd);
		write(parentFd, &finishFlag, 1);
	}
	return 0;
}
