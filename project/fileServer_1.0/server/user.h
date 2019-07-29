#ifndef __USER_H__
#define __USER_H__

#include "head.h"
#include "transFile.h"

//用户结点
typedef struct tag_user{
	int clientFd;
	int stat;//0离线，1在线
	char usrName[20];
	char usrPwd[20];
	char rootDir[30];
	char curDir[60];
}User_t, *pUser_t;

typedef struct tag_list_node{
	User_t user;
	struct tag_list_node *pNext;
}ListNode_t, *pListNode_t;

typedef struct{
	pListNode_t pHead;
	int listSize;
}List_t, *pList_t;

void listInit(pList_t pList);
void listHeadInsert(pList_t pList, pListNode_t pListNode);
int listDeleteByFd(pList_t pList, int fd);
int listQueryByFd(pList_t pList, int fd, pListNode_t *pListNode);
int listQueryByName(pList_t pList, char* name, pListNode_t *ppListNode);

void userInforLoad(pList_t pList, char* userInformatonFile);
void userInforSave(pList_t pList, char* userInformatonFile);

int userLoginVerify(pList_t pList, int newFd);
int userLoginRegister(pList_t pList, int newFd);
void userOffline(pList_t pList, int newFd);


#endif
