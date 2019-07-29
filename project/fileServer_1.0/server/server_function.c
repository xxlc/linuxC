#include "head.h"
#include "server_function.h"
#include "user.h"
#include "transFile.h"

int parsePacket(struct tag_task *pTask, void* packet){
	int type = *(int*)packet;
	pTask->args = (char*)packet+sizeof(int);
	switch(type){
		case COMMAND_CD:
			pTask->handle = handleCd;
			break;
		case COMMAND_LS:
			//pTask->handle = handleLs;
			break;
		case COMMAND_PUTS:
			//pTask->handle = handlePuts;
			break;
		case COMMAND_GETS:
			//pTask->handle = handleGets;
			break;
		case COMMAND_REMOVE:
			//pTask->handle = handleRemove;
			break;
		case COMMAND_PWD:
			//pTask->handle = handlePwd;
			break;
		default:
			printf("packet error\n");
			break;
	}
	return 0;
}

static char* detectInvalidDir(char *args){
	int i, len;
	//除去路径中最前面的'/'和最后面的'/'
	if(args[strlen(args)-1]=='/'){
		args[strlen(args)-1] = '\0';
	}
	if(args[0]=='/'){
		args += 1;//偏移到'/'之后
	}

	len = strlen(args);
	for(i=0; i<len; ){
		if(args[i]=='.'){
			if(i+1<len&&args[i+1]=='.'){
				if(i==0){
					//      ..    ../
					if(i+2 >= len){
						printf(" 非法字符 .. \n");
						return NULL;
					}
					if(args[i+2] == '/'){
						printf(" 非法字符 ../ \n");
						return NULL;
					}
				}
				if(args[i-1] == '/'){
					//      /..    /../
					if(i+2 >= len){
						printf(" 非法字符 .. \n");
						return NULL;
					}
					if(args[i+2] == '/'){
						printf(" 非法字符 ../ \n");
						return NULL;
					}
				}
				i = i+2;
			}
			if(i == 0){
				//      .     ./
				if(i+1 >= len){
					printf(" 非法字符 . \n");
					return NULL;
				}
				if(args[i+1]=='/'){
					printf(" 非法字符 ./ \n");
					return NULL;
				}
			}
			if(args[i-1] == '/'){
				//     /.     /./
				if(i+1 >= len){
					printf(" 非法字符 /. \n");
					return NULL;
				}
				if(args[i+1] == '/'){
					printf(" 非法字符 /./ \n");
					return NULL;
				}
			}
		}
		i++;
	}
	return args;
}
//cd
//用户端只能输入绝对路径名"/xx/学习/"
int handleCd(void *args, pUser_t pUser){
	char *targetDir;
	char dir[100]={0};
	struct stat buf;
	size_t error = 0;
	size_t packetLen;
		
	targetDir = detectInvalidDir((char*)args);
	if(NULL==targetDir){
		send(pUser->clientFd, (void*)&error, sizeof(size_t), 0);
		return -1;
	}
	sprintf(dir, "%s/%s", pUser->rootDir,targetDir);
	
	printf("targetDir=%s\n", targetDir);
	printf("dir = %s\n", dir);
	if( stat(dir, &buf) ){
		send(pUser->clientFd, (void*)&error, sizeof(size_t), 0);
		return -1;
	}
	if(S_ISDIR(buf.st_mode)){
		bzero(pUser->curDir, sizeof(pUser->curDir));
		if(strlen(targetDir)==0){
			sprintf(pUser->curDir, "/");
		}else{
			sprintf(pUser->curDir, "/%s/", targetDir);
		}
		//发送修改后的当前路径给客户端
		packetLen = strlen(pUser->curDir);
		send(pUser->clientFd, (void*)&packetLen, sizeof(size_t), 0);
		send(pUser->clientFd, (void*)pUser->curDir, packetLen, 0);
		return 0;
	}
	send(pUser->clientFd, (void*)&error, sizeof(size_t), 0);
	return -1;
}

//client
struct tag_prompt{
	char usrName[10];
	char curDir[80];
};
struct tag_request{
	size_t packetLen;
	int packetType;
	char packetData[100];
};
int requestCd(int serverFd, char *targetDir, char *curDir){
	int ret;
	char tempDir[80]={0};
	size_t packetLen;
	int packetType = COMMAND_CD;
	char *packetData = targetDir;
	packetLen = sizeof(int) + strlen(packetData);
	send(serverFd, (void*)&packetLen, sizeof(size_t), 0);
	send(serverFd, (void*)&packetType, sizeof(int), 0);
	send(serverFd, (void*)packetData, strlen(packetData), 0);
	
	ret = recvCycle(serverFd, (char*)&packetLen, sizeof(size_t));
	if(-1==ret){return -1;}
	ret = recvCycle(serverFd, (char*)tempDir, packetLen);
	if(-1==ret){return -1;}
	strcpy(curDir, tempDir);
	return 0;
}
//ls

//puts

//gets

//remove

//pwd

