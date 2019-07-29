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
			pTask->handle = handleLs;
			break;
		case COMMAND_PUTS:
			pTask->handle = handlePuts;
			break;
		case COMMAND_GETS:
			pTask->handle = handleGets;
			break;
		case COMMAND_REMOVE:
		//	pTask->handle = handleRemove;
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

//ls
int handleLs(void *args, pUser_t pUser){
	char *targetDir;
	char dir[100]={0};
	char lsInfo[2048]={0};
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
		// ls 目录名
		char path[1024];
		DIR *pDir;
		struct dirent *pDirInfo;
		char fileType, time[13]={0};
		int i = 0;
		pDir = opendir(dir);
		while( (pDirInfo=readdir(pDir))!=NULL ){
			if(!strcmp(pDirInfo->d_name,".")||!strcmp(pDirInfo->d_name,"..")){
				continue;
			}
			sprintf(path,"%s%c%s",dir,'/',pDirInfo->d_name);
			stat(path, &buf);
			memcpy(time, ctime(&buf.st_mtime)+4, 12);
			if(S_ISDIR(buf.st_mode)){
				fileType = 'd';
			}else{
				fileType = '-';
			}
			// 文件类型 文件名 大小 时间
			sprintf(lsInfo+i," %-3c%-20s %-5ld %-13s\n",fileType,pDirInfo->d_name,
					buf.st_size,time);
			i += 1+3+20+1+5+1+13+1;
		}
		if(0==i){
			packetLen = 1;
			strcpy(lsInfo, "\n");
			send(pUser->clientFd, (void*)&packetLen, sizeof(size_t), 0);
			send(pUser->clientFd, &lsInfo, packetLen, 0);
		}
		packetLen = i;
		send(pUser->clientFd, (void*)&packetLen, sizeof(size_t), 0);
		send(pUser->clientFd, &lsInfo, packetLen, 0);
	}else{
		send(pUser->clientFd, (void*)&error, sizeof(size_t), 0);
		return -1;
	}
	return 0;
}

//puts
int handlePuts(void *args, pUser_t pUser){
	int ret;
	size_t error = 0;
	size_t success = 1;
	send(pUser->clientFd, (void*)&success, sizeof(size_t), 0);
	printf("begin recv file\n");
	ret = recvFileByMmap(pUser->clientFd, pUser->rootDir,  pUser->curDir);
	if(-1==ret){
		send(pUser->clientFd, (void*)&error, sizeof(size_t), 0);
		return -1;
	}
	send(pUser->clientFd, (void*)&success, sizeof(size_t), 0);
	return 0;
}
//gets
int handleGets(void *args, pUser_t pUser){
	size_t error = 0;
	int ret;

	ret = sendFileByMmap(pUser->clientFd, (char*)args, pUser->rootDir);
	if(-1==ret){
		send(pUser->clientFd, (void*)&error, sizeof(size_t), 0);
		return -1;	
	}
	size_t success = 1;
	send(pUser->clientFd, (void*)&success, sizeof(size_t), 0);
	return 0;
}

//remove

//pwd

