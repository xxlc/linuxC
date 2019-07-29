#include "head.h"
#include "epoll_op.h"
#include "tcp_init.h"

#define SUCCESS 1
#define FAIL    0
#define SIZE_8M (1<<23)

char args[2][20];
char loginReq[256];

int recvCycle(int sfd,char* address,size_t size){
	size_t ret, total=0;
	while(total < size){
		ret = recv(sfd, address+total, size-total, 0);
		if(0==ret){return -1;}
		total += ret;
	}
	return total;
}

void* threadFunc(void* threadArgs){
	//fileInfo由三个字符串组成：文件名 文件code值 文件大小
	char fileInfo[3][40];
	sscanf((char*)threadArgs,"%s%s%s",fileInfo[0],fileInfo[1],fileInfo[2]);
	free(threadArgs);
	//tcp
	int sfd;
	clientTcpInit(&sfd, args[0], (unsigned short)atoi(args[1]));
	//login
	size_t loginLen = strlen(loginReq);
	int flag;
	send(sfd,&loginLen,sizeof(size_t),0);
	send(sfd,loginReq,loginLen,0);
	recvCycle(sfd, (char*)&loginLen, sizeof(size_t));
	recvCycle(sfd, (char*)&flag, loginLen);	
	if( FAIL==flag ){
		printf("thread error\n");
		pthread_exit(NULL);
	}
	//start down file
	size_t requestLen;
	char request[200];
	int fd;
	int pipeFd[2];
	struct stat st;
	char *fileName = fileInfo[0];
	char *code = fileInfo[1];
	off_t fileSize = (off_t)atol(fileInfo[2]);
	off_t curSize;
	char fileTemp[40];
	pipe(pipeFd);
	sprintf(fileTemp,"..%s.temp",fileName);//临时文件，文件未传输完整时保留
	fd = open(fileTemp,O_WRONLY|O_APPEND|O_SYNC|O_CREAT,0644);
	fstat(fd,&st);
	curSize = st.st_size;
	//getsfile code offset size---请求包内容
	while(fileSize-curSize >= SIZE_8M){
		//一次请求8M
		sprintf(request,"getsfile %s %lu %d ",code,curSize,SIZE_8M);
		requestLen = strlen(request);
		send(sfd,&requestLen,sizeof(size_t),0);
		send(sfd,request,requestLen,0);
		recvCycle(sfd, (char*)&requestLen, sizeof(size_t));
		recvCycle(sfd, (char*)&flag, requestLen);
		if(SUCCESS==flag){
			int total=0,ret;
			while(total < SIZE_8M){
				ret=splice(sfd,NULL,pipeFd[1],NULL,SIZE_8M-total,SPLICE_F_MOVE|SPLICE_F_MORE);
				splice(pipeFd[0],NULL,fd,NULL,ret,SPLICE_F_MOVE|SPLICE_F_MORE);
				curSize += ret;
				total += ret;
			}
		}else{
			printf("getsfile error\n");
			pthread_exit(NULL);
		}
	}
	if(fileSize-curSize > 0){
		//一次请求完
		sprintf(request,"getsfile %s %lu %lu ",code,curSize,fileSize-curSize);
		requestLen = strlen(request);
		send(sfd,&requestLen,sizeof(size_t),0);
		send(sfd,request,requestLen,0);
		recvCycle(sfd, (char*)&requestLen, sizeof(size_t));
		recvCycle(sfd, (char*)&flag, requestLen);
		if(SUCCESS==flag){
			int ret;
			while(fileSize-curSize > 0){
				ret=splice(sfd,NULL,pipeFd[1],NULL,fileSize-curSize,SPLICE_F_MOVE|SPLICE_F_MORE);
				splice(pipeFd[0],NULL,fd,NULL,ret,SPLICE_F_MOVE|SPLICE_F_MORE);
				curSize += ret;
			}
		}else{
			printf("getsfile error\n");
			pthread_exit(NULL);
		}
	}
	rename(fileTemp,fileName);
	pthread_exit(NULL);
}

int main(){
	FILE* fp = fopen("./conf/config", "rb");
	fscanf(fp, "%s%s", args[0],args[1]);
	fclose(fp);
#ifdef DEBUG
	printf("ip=%s,port=%s\n", args[0],args[1]);
#endif
	//tcp
	int sfd;
	clientTcpInit(&sfd, args[0], (unsigned short)atoi(args[1]));
	//request
	size_t requestLen;
	char request[256];
	char command[30];
	//reply
	size_t replyLen;
	char reply[1024];
	int flag;
	//login or register
	printf("please select login or register:\n");
	printf("login/register [user name] [password]\n");
	printf("-----------------------------------------------------------\n");
	while(1){
		bzero(request,sizeof(request));
		fgets(request, sizeof(request)-1, stdin);
		requestLen = strlen(request);
		request[requestLen-1] = ' ';
		sscanf(request,"%s",command);
		//send request data packet
		send(sfd,&requestLen,sizeof(size_t),0);
		send(sfd,request,requestLen,0);
		recvCycle(sfd, (char*)&replyLen, sizeof(size_t));
		recvCycle(sfd, (char*)&flag, replyLen);
		if( SUCCESS==flag ){
				if( !strcmp(command,"login") ){
					printf("login success\n");
					strcpy(loginReq,request);
					break;
				}else{
					printf("register success\n");
				}
		}else{
			printf("command failed\n");
		}
	}
	//cd ls pwd puts gets
	while(1){
		printf("netDisk >> ");
		bzero(request,sizeof(request));
		fgets(request, sizeof(request)-1, stdin);
		requestLen = strlen(request);
		request[requestLen-1] = ' ';
		sscanf(request,"%s",command);
		printf("request=%s,command=%s\n",request,command);
		if( !strcmp(command,"ls")||!strcmp(command,"pwd") ){
/*ls--pwd--------------------------------------------------------------------------------*/
			send(sfd,&requestLen,sizeof(size_t),0);
			send(sfd,request,requestLen,0);
			recvCycle(sfd, (char*)&replyLen, sizeof(size_t));
			recvCycle(sfd, (char*)&flag, replyLen);
			if(SUCCESS==flag){
				bzero(reply,sizeof(reply));
				recvCycle(sfd, (char*)&replyLen, sizeof(size_t));
				recvCycle(sfd, reply, replyLen);
				printf("%sreplyLen=%lu\n",reply,replyLen);
			}else{
				printf("%s invalid\n",command);
			}
		}else if( !strcmp(command,"puts") ){
/*puts-----------------------------------------------------------------------------------*/

		}else if( !strcmp(command,"gets") ){
/*gets-----------------------------------------------------------------------------------*/
			send(sfd,&requestLen,sizeof(size_t),0);
			send(sfd,request,requestLen,0);
			recvCycle(sfd, (char*)&replyLen, sizeof(size_t));
			recvCycle(sfd, (char*)&flag, replyLen);
			if(SUCCESS==flag){
				bzero(reply,sizeof(reply));
				recvCycle(sfd, (char*)&replyLen, sizeof(size_t));
				char *fileInfo=(char*)calloc(1,replyLen+1);
				recvCycle(sfd, fileInfo, replyLen);
				printf("%sreplyLen=%lu\n",reply,replyLen);
				int fd = open(reply,O_RDWR);
				if(fd!=-1){printf("当前目录存在同名文件\n");close(fd);continue;}
				pthread_t thid;
				pthread_create(&thid,NULL,threadFunc,fileInfo);
			}else{
				printf("%s invalid\n",command);
			}
		}else{
/*cd--remove---非法命令------------------------------------------------------------------*/
			//cd remove 违法命令,这三者命令无返回数据
			send(sfd,&requestLen,sizeof(size_t),0);
			send(sfd,request,requestLen,0);
			recvCycle(sfd, (char*)&replyLen, sizeof(size_t));
			recvCycle(sfd, (char*)&flag, replyLen);
			if(SUCCESS==flag){
				printf("%s success\n",command);
			}else{
				printf("command invalid\n");	
			}
		}
	}
	return 0;
}
