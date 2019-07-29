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
	int clientFd;
	char fileName[] = "file";
	char finishFlag;
	while(1){
		recvFd(parentFd, &clientFd);
		//拿到clientfd后开始工作
		printf("I am child process. I am working.\n");
		int fileFd = open(fileName, O_RDWR);
		struct stat fileStat;
		fstat(fileFd, &fileStat);
		//发送文件名长度及文件名
		int dataLen = strlen(fileName);
		send(clientFd, &dataLen, 4, 0); 
		send(clientFd, fileName, dataLen, 0); 
		//发送文件大小
		dataLen = sizeof(fileStat.st_size);
		send(clientFd, &dataLen, 4, 0); 
		send(clientFd, &fileStat.st_size, dataLen, 0); 
		printf("start send %s total=%lu\n", fileName, fileStat.st_size);
		//用sendfile接口发送文件
		sendfile(clientFd, fileFd, NULL, fileStat.st_size);

		close(clientFd);
		write(parentFd, &finishFlag, 1);
	}
	return 0;
}
