#include "user.h"

void listInit(pList_t pList){
	pList->pHead = NULL;	
	pList->listSize = 0;
}

void listHeadInsert(pList_t pList, pListNode_t pListNode){
	if(NULL==pList->pHead){
		pList->pHead = pListNode;
		pListNode->pNext = NULL;
	}else{
		pListNode->pNext = pList->pHead;
		pList->pHead = pListNode;
	}
	pList->listSize++;
}

int listDeleteByFd(pList_t pList, int fd){
	pListNode_t pPast,pCur;
	if(pList->listSize==0){
		return -1;
	}
	pPast = pCur = pList->pHead;
	while(pCur&&pCur->user.clientFd!=fd){
		pPast = pCur;
		pCur = pCur->pNext;
	}
	if(NULL==pCur){
		return -1;
	}
	if(pPast==pCur){
		pList->pHead = pCur->pNext;
	}else{
		pPast->pNext = pCur->pNext;
	}
	free(pCur);
	pList->listSize--;
	return 0;
}

int listQueryByFd(pList_t pList, int fd, pListNode_t *ppListNode){
	pListNode_t pCur = pList->pHead;
	while(NULL!=pCur){
		if(pCur->user.clientFd==fd){
			*ppListNode = pCur;
			return 0;
		}	
		pCur = pCur->pNext;
	}
	return -1;
}
int listQueryByName(pList_t pList, char* name, pListNode_t *ppListNode){
	pListNode_t pCur = pList->pHead;
	while(NULL!=pCur){
		if(strcmp(pCur->user.usrName,name)==0){
			*ppListNode = pCur;
			return 0;
		}
		pCur = pCur->pNext;
	}
	return -1;
}

void userInforLoad(pList_t pList, char* userInformatonFile){
	FILE* fp = fopen(userInformatonFile, "rb");
	pListNode_t pListNode;
	pUser_t pNewUser;
	pListNode = (pListNode_t)calloc(1, sizeof(ListNode_t));
	pNewUser = &pListNode->user;
	while( fscanf(fp, "%s%s%s",pNewUser->usrName,pNewUser->usrPwd,
				pNewUser->rootDir)!=EOF ){
		printf("%s$%s$%s\n",pNewUser->usrName,pNewUser->usrPwd,
				pNewUser->rootDir);
		pNewUser->clientFd = -1;
		strcpy(pNewUser->curDir, "/");
		listHeadInsert(pList, pListNode);
		pListNode = (pListNode_t)calloc(1, sizeof(ListNode_t));
		pNewUser = &pListNode->user;
	}
	printf("user information init success\n");
	free(pListNode);
	fclose(fp);
}

void userInforSave(pList_t pList, char* userInformatonFile){
	FILE* fp = fopen(userInformatonFile, "wb");	
	pListNode_t pCur = pList->pHead;
	while(pCur){
		fprintf(fp,"%-20s%-20s%s\n",pCur->user.usrName,pCur->user.usrPwd,
				pCur->user.rootDir);
		pCur = pCur->pNext;
	}
}

int userLoginVerify(pList_t pList, int newFd){
	int ret;
	size_t packetLen;
	char userName[20], userPwd[20];
	pListNode_t pListNode;
	//userName
	ret = recvCycle(newFd, (char*)&packetLen, sizeof(size_t));                
	if(-1==ret||packetLen>=20){return -1;}
	bzero(userName, sizeof(userName));
	ret = recvCycle(newFd, userName, packetLen);               
   	printf("userName=%s\n", userName);	
   	if(-1==ret||-1==listQueryByName(pList,userName,&pListNode)){return -1;}
	//passwd
	ret = recvCycle(newFd, (char*)&packetLen, sizeof(size_t));
	if(-1==ret||packetLen>=20){return -1;}
	bzero(userPwd, sizeof(userPwd));
	ret = recvCycle(newFd, userPwd, packetLen);
	printf("passwd=%s\n",userPwd);	
	if( -1==ret||strcmp(pListNode->user.usrPwd, userPwd)!=0 ){return -1;}
	pListNode->user.clientFd = newFd;
	printf("rootDir=%s\n", pListNode->user.rootDir);
	strcpy(pListNode->user.curDir,"/");
	pListNode->user.stat = 1;
	ret = 1;
	send(newFd, (void*)&ret, sizeof(int), 0);//发送任意四个字节的回包，表示登录成功
	return 0;
}
int userLoginRegister(pList_t pList, int newFd){
	int ret;
	size_t packetLen;
	char userName[20], userPwd[20];
	pListNode_t pListNode;
	//userName  接收失败或有重复用户名则return 0
	ret = recvCycle(newFd, (char*)&packetLen, sizeof(size_t));                
	if(-1==ret||packetLen>=20){return -1;}
	bzero(userName, sizeof(userName));
	ret = recvCycle(newFd, userName, packetLen);               
   	if(-1==ret||0==listQueryByName(pList,userName,&pListNode)){return -1;}   
	//passwd
	ret = recvCycle(newFd, (char*)&packetLen, sizeof(size_t));
	if(-1==ret||packetLen>=20){return -1;}
	bzero(userPwd, sizeof(userPwd));
	ret = recvCycle(newFd, userPwd, packetLen);               
	if( -1==ret ){return -1;}//passwd接收失败返回-1
	//创建新的用户结点
	pListNode = (pListNode_t)calloc(1, sizeof(ListNode_t));
	strcpy(pListNode->user.usrName,userName);
	strcpy(pListNode->user.usrPwd,userPwd);
	sprintf(pListNode->user.rootDir,"./userData/%s",userName);
	mkdir(pListNode->user.rootDir, 0775);
	pListNode->user.clientFd = newFd;
	strcpy(pListNode->user.curDir,"/");
	pListNode->user.stat = 1;
	listHeadInsert(pList, pListNode);
	printf("new user register\n");
	ret = 1;
	send(newFd, (void*)&ret, sizeof(int), 0);//发送任意四个字节的回包，表示注册成功
	return 0;	
}
void userOffline(pList_t pList, int newFd){
	pListNode_t pListNode;
	listQueryByFd(pList, newFd, &pListNode);
	pListNode->user.clientFd = -1;
	pListNode->user.stat = 0;
}
