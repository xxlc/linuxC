#include "head.h" 
#include "transFile.h"

int sendFileByMmap(int targetFd, char *fileName){
	int fileFd = open(fileName, O_RDWR);
	struct stat fileStat;
	fstat(fileFd, &fileStat);
	//发送文件名长度及文件名
	int dataLen = strlen(fileName);
	send(targetFd, &dataLen, 4, 0);
	send(targetFd, fileName, dataLen, 0);
	//发送文件大小
	dataLen = sizeof(fileStat.st_size);
	send(targetFd, &dataLen, 4, 0);
	send(targetFd, &fileStat.st_size, dataLen, 0);
	printf("start send %s total=%lu\n", fileName, fileStat.st_size);
	//通过mmap发送整个文件
	char *p=(char*)mmap(NULL, fileStat.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fileFd, 0);
	if((char*)-1==p){
		printf("mmap");
		return -1;
	}
	int ret = send(targetFd, p, fileStat.st_size, 0);
	if(-1==ret){
		munmap(p, fileStat.st_size);
		close(fileFd);
		perror("transFileByMmap send");
		return -1;
	}
	munmap(p, fileStat.st_size);
	close(fileFd);
	printf("send %s success\n", fileName);
	return 0;
}

int recvCycle(int sfd, char *addr, size_t size){
	size_t total=0, ret;
	while(total<size){
		ret = recv(sfd, addr+total, size-total, 0); 
		if(0==ret){
			printf("recvFileByMmap failed. the return of recv is zero\n");
			return -1; 
		}   
		total += ret;
	}   
	return 0;
}

int recvFileByMmap(int targetFd){
	char fileName[100] = {0};
	int dataLen;
	size_t fileSize;
	//文件名
	recvCycle(targetFd, (char*)&dataLen, 4); 
	recvCycle(targetFd, fileName, dataLen);
	//文件大小
	recvCycle(targetFd, (char*)&dataLen, 4); 
	recvCycle(targetFd, (char*)&fileSize, dataLen);
	//通过mmap接收整个文件
	int fd = open(fileName, O_CREAT|O_RDWR, 0644);
	ftruncate(fd, fileSize);
	char *p = (char*)mmap(NULL, fileSize, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0); 
	if((char*)-1==p){
		printf("recvFileByMmap mmap failed\n");
		return -1;
	}
	//开始接收文件
	printf("start receive %s total=%lu\n", fileName, fileSize);
	time_t startTime, endTime;
	time(&startTime);
	size_t splice, lastLoadSize, downLoadSize, ret;
	splice = fileSize/100;
	downLoadSize = lastLoadSize = 0;
	while(downLoadSize<fileSize){
		if(downLoadSize-lastLoadSize>splice){
			lastLoadSize = downLoadSize;
			printf("%5.2f%c\r", (float)downLoadSize/fileSize*100, '%');
			fflush(stdout);
		}
		ret = recv(targetFd, p+downLoadSize, fileSize-downLoadSize, 0);
		if(ret==0){
			printf("recvFileByMmap failed. the return of recv is zero\n");
			munmap(p, fileSize);
			close(fd);
			return -1;
		}
		downLoadSize += ret;
	}
	time(&endTime);
	munmap(p, fileSize);
	close(fd);
	printf("recv %s success, spend time %ld s\n", fileName, endTime-startTime);
	return 0;
} 

int handleTransFile(void *args){
	struct file_node *pFileReq = (struct file_node*)args;
	sendFileByMmap(pFileReq->clientFd, pFileReq->fileName);
	return 0;
}
