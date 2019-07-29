#include "client_function.h"

int userLogin(int *psfd, char *ip, unsigned short port, char *pName){
	char userName[20],passwd[20], ch;
	int i, type=1;
	size_t len;
	printf("用户名不能含有除A-Z、a-z及下划线以外的字符\n");
	while(1){
start:
		printf("用户名:");
		i = 0;
		while(scanf("%c",&ch)&&ch!='\n'){
			if(i>=sizeof(userName)-1){
				printf("用户名超过最长限度\n");
				while(scanf("%c",&ch)&&ch!='\n');//清空缓冲
				goto start;
			}
			if( (ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||ch=='_' ){
				userName[i++] = ch;
			}else{
				printf("用户名中含有非法字符\n");
				while(scanf("%c",&ch)&&ch!='\n');//清空缓冲
				goto start;
			}
		}
		userName[i] = '\0';
		printf("密码:");
		i = 0;
		while(scanf("%c",&ch)&&ch!='\n'){
			if(i>=sizeof(passwd)-1){
				while(scanf("%c",&ch)&&ch!='\n');//清空缓冲
				return -1;
			}
			passwd[i++] = ch;
		}
		passwd[i] = '\0';
		clientTcpInit(psfd, ip, port);
		send(*psfd, (void*)&type, sizeof(int), 0);
		len = strlen(userName);
		send(*psfd, (void*)&len, sizeof(size_t), 0);	
		send(*psfd, (void*)userName, len, 0);
		len = strlen(passwd);
		send(*psfd, (void*)&len, sizeof(size_t), 0);
		send(*psfd, (void*)passwd, len, 0);
		recvCycle(*psfd, (char*)&type, sizeof(int));
		if(0==type){return -1;}
		strcpy(pName, userName);
		return 0;
	}
}
int userRegister(int *psfd, char *ip, unsigned short port, char *pName){
	char userName[20],passwd[20], ch;
	int i, type=2;
	size_t len;
	printf("用户名不能含有除A-Z、a-z及下划线以外的字符\n");
	while(1){
start:
		printf("新用户名:");
		i = 0;
		while(scanf("%c",&ch)&&ch!='\n'){
			if(i>=sizeof(userName)-1){
				printf("用户名超过最长限度\n");
				while(scanf("%c",&ch)&&ch!='\n');//清空缓冲
				goto start;
			}
			if( (ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')||ch=='_' ){
				userName[i++] = ch;
			}else{
				printf("用户名中含有非法字符\n");
				while(scanf("%c",&ch)&&ch!='\n');//清空缓冲
				goto start;
			}
		}
		userName[i] = '\0';
		printf("新密码:");
		i = 0;
		while(scanf("%c",&ch)&&ch!='\n'){
			if(i>=sizeof(passwd)-1){
				while(scanf("%c",&ch)&&ch!='\n');//清空缓冲
				return -1;
			}
			passwd[i++] = ch;
		}
		passwd[i] = '\0';
		clientTcpInit(psfd, ip, port);
		send(*psfd, (void*)&type, sizeof(int), 0);
		len = strlen(userName);
		send(*psfd, (void*)&len, sizeof(size_t), 0);	
		send(*psfd, (void*)userName, len, 0);
		len = strlen(passwd);
		send(*psfd, (void*)&len, sizeof(size_t), 0);
		send(*psfd, (void*)passwd, len, 0);
		recvCycle(*psfd, (char*)&type, sizeof(int));
		if(0==type){return -1;}
		strcpy(pName, userName);
		return 0;
	}
}

int commandScanf(struct tag_request *pReq, char *curDir){
	int i,flag, len;
	char command[10], args[95], ch;

	i = flag = 0;
	len = sizeof(command)-1;	
	while( scanf("%c", &ch) && i<len ){
		if(ch==' '&&flag==0)//除去字符串前面空格
			continue;
		if(ch==' '&&flag==1)//代表一个字符串读取完毕，退出
			break;
		if(ch!=' '&&flag==0)//读到字符串首个字符，将标记置1
			flag = 1;
		if('\n'==ch){
			break;
		}
		command[i++] = ch;
	}
	command[i] = '\0';
	//printf("command=%s\n", command);
	if( strcmp(command, "cd")==0 ){
		pReq->packetType = COMMAND_CD;
	}else if( strcmp(command, "ls")==0 ){
		pReq->packetType = COMMAND_LS;
	}else if( strcmp(command, "puts")==0 ){
		pReq->packetType = COMMAND_PUTS;
	}else if( strcmp(command, "gets")==0 ){
		pReq->packetType = COMMAND_GETS;
	}else if( strcmp(command, "remove")==0 ){
		pReq->packetType = COMMAND_REMOVE;
	}else if( strcmp(command, "pwd")==0 ){
		pReq->packetType = COMMAND_PWD;
	}else{
		//清空输入缓冲区
		while( scanf("%c",&ch) && ch!='\n' );
		return -1;
	}
	//printf("-------command args---------\n");
	if(pReq->packetType==COMMAND_PUTS){
		bzero(pReq->packetData, sizeof(pReq->packetData));
		scanf("%s", pReq->packetData);
		printf("PUTS %s\n", pReq->packetData);
		while( scanf("%c",&ch) && ch!='\n' );
		return 0;
	}
	//读入命令参数
	if('\n'==ch){
		strcpy(pReq->packetData, curDir);
		return 0;
	}
	i = flag = 0;
	len = sizeof(args)-1;
	while( scanf("%c",&ch) && i<len ){
		if(ch==' '&&flag==0)//除去字符串前面空格
			continue;
		if(ch==' '&&flag==1)//代表一个字符串读取完毕，退出
			break;
		if(ch=='\n')
			break;
		if(ch!=' '&&flag==0)//读到字符串首个字符，将标记置1
			flag = 1;
		args[i++] = ch;
	}
	if(0==i){
		strcpy(pReq->packetData, curDir);
		return 0;
	}
	args[i] = '\0';
	if(args[0]=='/'){
		strcpy(pReq->packetData, args);
	}else{
		sprintf(pReq->packetData, "%s%s", curDir, args);
	}
	//printf("args=%s\n", pReq->packetData);
	if(ch=='\n'){
		return 0;
	}else{
		//清空输入缓冲区
		while( scanf("%c",&ch) && ch!='\n' );
		return 0;
	}
}

int requestCd(int serverFd, struct tag_request *pReq, char *curDir){
	int ret;
	size_t packetLen;
	char tempDir[80]={0};
	send(serverFd, (void*)&pReq->packetLen, sizeof(size_t), 0); 
	send(serverFd, (void*)&pReq->packetType, sizeof(int), 0); 
	send(serverFd, (void*)pReq->packetData, strlen(pReq->packetData), 0); 

	//printf("等待回复\n");
	ret = recvCycle(serverFd, (char*)&packetLen, sizeof(size_t));
	if(-1==ret){return -1;}
	ret = recvCycle(serverFd, (char*)tempDir, packetLen);
	if(-1==ret){return -1;}
	strcpy(curDir, tempDir);
	return 0;
}

int requestLs(int serverFd, struct tag_request *pReq){
	int ret;
	size_t packetLen;
	char lsInfo[2048]={0};
	send(serverFd, (void*)&pReq->packetLen, sizeof(size_t), 0); 
	send(serverFd, (void*)&pReq->packetType, sizeof(int), 0); 
	send(serverFd, (void*)pReq->packetData, strlen(pReq->packetData), 0); 

	//printf("等待回复\n");
	ret = recvCycle(serverFd, (char*)&packetLen, sizeof(size_t));
	if(-1==ret){return -1;}
	ret = recvCycle(serverFd, (char*)lsInfo, packetLen);
	if(-1==ret){return -1;}
	printf("%s",lsInfo);
	return 0;
}

int requestPuts(int serverFd, struct tag_request *pReq){
	int ret;
	size_t packetLen = 2*sizeof(int);
	printf("requestPuts packetLen=%lu,packetType=%d\n", packetLen,pReq->packetType);
	send(serverFd, (void*)&packetLen, sizeof(size_t), 0);
	send(serverFd, (void*)&pReq->packetType, sizeof(int), 0);
	send(serverFd, (void*)&pReq->packetType, sizeof(int), 0);
	
	ret = recvCycle(serverFd, (char*)&packetLen, sizeof(size_t));
	if(-1==ret||packetLen==0){return -1;}
	printf("begin send file\n");
	ret = sendFileByMmap(serverFd, pReq->packetData);	
	if(-1==ret){return -1;}
	ret = recvCycle(serverFd, (char*)&packetLen, sizeof(size_t));
	if(-1==ret||packetLen==0){return -1;}
	return 0;
}

int requestGets(int serverFd, struct tag_request *pReq){
	int ret;
	size_t packetLen;
	send(serverFd, (void*)&pReq->packetLen, sizeof(size_t), 0); 
	send(serverFd, (void*)&pReq->packetType, sizeof(int), 0); 
	send(serverFd, (void*)pReq->packetData, strlen(pReq->packetData), 0); 
	
	ret = recvFileByMmap(serverFd);
	if(-1==ret){return -1;}
	ret = recvCycle(serverFd, (char*)&packetLen, sizeof(size_t));
	if(-1==ret||packetLen==0){return -1;}
	return 0;
}

int requestPwd(char *curDir){
	printf("%s\n", curDir);
	return 0;
}
