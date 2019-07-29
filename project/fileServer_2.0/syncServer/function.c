#include "function.h"

static void sendFlag(int sfd,int flag){
	size_t len = sizeof(int);
	send(sfd,&len,sizeof(size_t),0);
	if(0==flag){
		//fail send 0
		send(sfd,&flag,len,0);
		return;
	}
	if(1==flag){
		//success send 1
		send(sfd,&flag,len,0);
		return;
	}
	printf("sendFlag() arguments error\n");
}

static int recvCycle(int sfd,char* address,size_t size){
	size_t ret, total=0;
	while(total < size){
		ret = recv(sfd, address+total, size-total, 0);
		if(0==ret){return -1;}
		total += ret;
	}
	return total;
}

static void addLog(MYSQL *conn,char *arg1,char *arg2){
	char query[100];
	sprintf(query,"insert into op_log(request) values('%s %s');",arg1,arg2);
	mysql_query(conn,query);
}

static void clean(void* args){ 
	pthread_mutex_unlock((pthread_mutex_t*)args);
}
void* pthreadFunc(void* args){
	pFactory_t pFactory = (pFactory_t)args;
	pthread_cond_t *pCond = &pFactory->cond;
	pQue_t pq = &pFactory->que;
	pthread_mutex_t *pMutex = &pq->mutex;
	unsigned int *dirCode = pFactory->dirCode;
	int *fileDes = pFactory->fileDes;
	pNode_t pTaskNode;
	int ret;
	//databases connect
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char server[] = "127.0.0.1";
	char user[] = "root";
	char password[] = "jiawei&i";
	char database[] = "file_manage_database";
	char query[300];
	conn = mysql_init(NULL);
	printf("%p\n",conn);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		printf("Error, can't connect databases :%s\n",mysql_error(conn));
		pthread_exit(NULL);
	}
	printf("connect database\n");
	//request
	int newFd;
	size_t requestLen;
	char request[256],arguments[4][80];
	while(1){
		pthread_mutex_lock(pMutex);
		pthread_cleanup_push(clean, pMutex);
		if(0==pq->queSize){
			pthread_cond_wait(pCond, pMutex);
		}   
		ret = deQueue(pq, &pTaskNode);
		pthread_cleanup_pop(1);
		if(-1==ret){printf("deQueue failed\n");continue;}
		newFd = pTaskNode->clientFd;
		free(pTaskNode);
		//任务开始
#ifdef DEBUG
		printf("---------------------------------------start\n");
#endif
		bzero(request,sizeof(request));
		ret = recvCycle(newFd, (char*)&requestLen, sizeof(size_t));
		if(-1==ret){printf("client closed\n");dirCode[newFd]=0;close(newFd);continue;}
		ret = recvCycle(newFd, (char*)&request, requestLen);
		if(-1==ret){printf("client closed\n");dirCode[newFd]=0;close(newFd);continue;}
		bzero(arguments,sizeof(arguments));
		sscanf(request,"%s%s%s%s",arguments[0],arguments[1],arguments[2],arguments[3]);
#ifdef DEBUG
		printf("request: %s\n",request);
#endif
		if(!strcmp(arguments[0],"login")){
/*login---------------------------------------------------------------------------*/
			char *name,*password;
			name = arguments[1];
			password = arguments[2];
			sprintf(query,"select 1 from user_information where name='%s' and password='%s' limit 1;",name,password);
#ifdef DEBUG
			printf("%s\n",query);
#endif
			ret = mysql_query(conn,query);
			if(0==ret){
				res = mysql_use_result(conn);
				row = mysql_fetch_row(res);
				mysql_free_result(res);
				if(NULL!=row){
					sprintf(query,"select code from user_file where owner='%s' and fileName='~' and precode=0 limit 1;", name);
#ifdef DEBUG
					printf("%s\n",query);
#endif
					mysql_query(conn,query);
					res = mysql_use_result(conn);
					row = mysql_fetch_row(res);
					dirCode[newFd] = (unsigned int)atoi(row[0]);
#ifdef	DEBUG
					printf("new client login success,row[0]=%s\n",row[0]);
#endif
					mysql_free_result(res);//用完free
					sendFlag(newFd,SUCCESS);
				}else{
					sendFlag(newFd,FAIL);
				}
			}else{
				sendFlag(newFd,FAIL);
			}
		}else if(!strcmp(arguments[0],"register")){
/*register---------------------------------------------------------------------------*/
			char *name,*password;
			name = arguments[1];
			password = arguments[2];
			sprintf(query,"insert into user_information(name,password) values('%s','%s');",name,password);
#ifdef DEBUG
			printf("%s\n",query);
#endif
			ret = mysql_query(conn,query);
			if(0==ret){
				sprintf(query,"insert into user_file(precode,owner,fileName,fileType,fileSize) values(0,'%s','~','d',4096);",name);
#ifdef DEBUG
				printf("%s\n",query);
#endif
				mysql_query(conn,query);
				sendFlag(newFd,SUCCESS);
			}else{
				sendFlag(newFd,FAIL);
			}
		}else if(!strcmp(arguments[0],"cd") && dirCode[newFd]!=0){
/*cd---------------------------------------------------------------------------*/
			addLog(conn,arguments[0],arguments[1]);
			char *argu = arguments[1];
			if(!strcmp(argu,"/")||!strcmp(argu,"~")){
				sprintf(query,"select owner from user_file where code=%u limit 1;", dirCode[newFd]);
#ifdef DEBUG
				printf("%s\n",query);
#endif
				mysql_query(conn,query);
				res = mysql_use_result(conn);
				row = mysql_fetch_row(res);
				sprintf(query,"select code from user_file where owner='%s' and fileName='~';", row[0]);
				mysql_free_result(res);//用完free
#ifdef DEBUG
				printf("%s\n",query);
#endif
				mysql_query(conn,query);
				res = mysql_use_result(conn);
				row = mysql_fetch_row(res);
				dirCode[newFd] = (unsigned int)atoi(row[0]);
				mysql_free_result(res);
				sendFlag(newFd,SUCCESS);	
			}else if(!strcmp(argu,"../")||!strcmp(argu,"..")){
				sprintf(query,"select precode from user_file where code=%u limit 1;", dirCode[newFd]);
#ifdef DEBUG
				printf("%s\n",query);
#endif
				mysql_query(conn,query);
				res = mysql_use_result(conn);
				row = mysql_fetch_row(res);
				if(NULL!=row){
					dirCode[newFd] = (unsigned int)atoi(row[0]);
					sendFlag(newFd,SUCCESS);
				}else{
					sendFlag(newFd,FAIL);
				}
				mysql_free_result(res);
			}else{
				sprintf(query,"select code from user_file where precode=%u and fileName='%s' and fileType='d' limit 1;", dirCode[newFd],argu);
#ifdef DEBUG
				printf("%s\n",query);
#endif
				ret = mysql_query(conn,query);
				if(0==ret){
					res = mysql_use_result(conn);
					row = mysql_fetch_row(res);
					if(NULL!=row){
						dirCode[newFd] = (unsigned int)atoi(row[0]);
						sendFlag(newFd,SUCCESS);
					}else{
						sendFlag(newFd,FAIL);
					}
					mysql_free_result(res);
				}else{
					sendFlag(newFd,FAIL);
				}
			}	
		}else if(!strcmp(arguments[0],"ls") && dirCode[newFd]!=0){
/*ls---------------------------------------------------------------------------*/
			addLog(conn,arguments[0],arguments[1]);
			size_t replyLen;
			char reply[1024]={0}, replyRow[50]={0};
			sprintf(query,"select fileTime,fileType,fileSize,fileName from user_file where precode=%u;", dirCode[newFd]);	
			mysql_query(conn,query);
			res = mysql_use_result(conn);
			if(NULL==res){
				sendFlag(newFd,FAIL);
			}else{
				while( (row=mysql_fetch_row(res))!=NULL ){
					//时间 文件类型 文件大小 文件名
					sprintf(replyRow,"%s %s %s\t %s\n",row[0],row[1],row[2],row[3]);
					strncat(reply,replyRow,strlen(replyRow));
				}
				mysql_free_result(res);
				replyLen = strlen(reply);
#ifdef DEBUG
				printf("%sreplyLen=%lu\n",reply,replyLen);
#endif
				sendFlag(newFd,SUCCESS);
				send(newFd,&replyLen,sizeof(size_t),0);
				send(newFd,reply,replyLen,0);
			}
		}else if(!strcmp(arguments[0],"puts") && dirCode[newFd]!=0){
/*puts---------------------------------------------------------------------------*/
		
		}else if(!strcmp(arguments[0],"getsfile") && dirCode[newFd]!=0){
/*getsfile---------------------------------------------------------------------------*/
			//在客户端拿到文件大小及code值后，发一个新的连接，并随后发出getsfile文件的请求
			//服务器先判断socketFd的当前dirCode值是否为文件code值，不是则将dirCode切换
			//切换成功才能响应请求，若dirCode=code，则直接响应
			unsigned int fileCode = (unsigned int)atol(arguments[1]);
			off_t offset = (off_t)atol(arguments[2]);
			off_t size = (off_t)atol(arguments[3]);
			if(dirCode[newFd]==fileCode){
				if(fileDes[fileCode]==0){
					//打开相应文件描述符
					//准备写
				}
				sendFlag(newFd,SUCCESS);
				//发送文件
			}else{
				sprintf(query,"select owner from user_file where code=%u limit 1;",dirCode[newFd]);
				mysql_query(conn,query);
				res = mysql_use_result(conn);
				row = mysql_fetch_row(res);
				sprintf(query,"select 1 from user_file where code=%u and owner='%s' limit 1;",fileCode,row[0]);
				mysql_free_result(res);
				mysql_query(conn,query);
				res = mysql_use_result(conn);
				if(NULL!=res){
					dirCode[newFd] = fileCode;
					if(fileDes[newFd]==0){
						//打开相应的文件描述符
						//准备写
					}
					sendFlag(newFd,SUCCESS);
					//发送文件
				}else{
					sendFlag(newFd,FAIL);
				}
			}
		}else if(!strcmp(arguments[0],"gets") && dirCode[newFd]!=0){
/*gets--------------获取文件大小及文件code值---------------------------------------*/
			addLog(conn,arguments[0],arguments[1]);
			size_t replyLen;
			char reply[128] = {0};
			char *fileName = arguments[1];
			sprintf(query,"select code,fileSize from where fileType='f' and fileName='%s' and procode=%u;",fileName,dirCode[newFd]);
			ret = mysql_query(conn,query);
			if(0==ret){
				res = mysql_use_result(conn);
				row = mysql_fetch_row(res);
				//文件名 文件code值 文件大小
				sprintf(reply,"%s %s %s\n",fileName,row[0],row[1]);
				replyLen = strlen(reply);
#ifdef DEBUG
				printf("%s",reply);
#endif
				sendFlag(newFd,SUCCESS);
				send(newFd,&replyLen,sizeof(size_t),0);
				send(newFd,reply,replyLen,0);
			}else{
				sendFlag(newFd,FAIL);
			}
		}else if(!strcmp(arguments[0],"remove") && dirCode[newFd]!=0){
/*remove---------------------------------------------------------------------------*/

		}else if(!strcmp(arguments[0],"pwd") && dirCode[newFd]!=0){
/*pwd---------------------------------------------------------------------------*/
			//通过将当前目录压栈，依次precode压栈，再依次出栈 + /
			addLog(conn,arguments[0],arguments[1]);
			size_t replyLen;
			char reply[128]={0};
			unsigned int curCode = dirCode[newFd];
			char stack[10][10];
			int i;
			for(i=0; i<10&&curCode!=0; i++){
				sprintf(query,"select precode,fileName from user_file where code=%u limit 1;", curCode);
				mysql_query(conn,query);
				res = mysql_use_result(conn);
				row = mysql_fetch_row(res);
				curCode = (unsigned int)atoi(row[0]);
				sprintf(stack[i],"%s/",row[1]);//压栈
				mysql_free_result(res);//释放空间
			}
			for(i=i-1 ; i>=0; i--){
				strncat(reply,stack[i],strlen(stack[i]));//出栈
			}
			strncat(reply,"\n",1);
			replyLen = strlen(reply);
#ifdef DEBUG
			printf("%sreplyLen=%lu\n",reply,replyLen);
#endif
			sendFlag(newFd,SUCCESS);
			send(newFd,&replyLen,sizeof(size_t),0);
			send(newFd,reply,replyLen,0);
		}else{
/*error---------------------------------------------------------------------------*/
			addLog(conn,arguments[0],arguments[1]);
			sendFlag(newFd,FAIL);
		}
		epollAdd(pFactory->epfd, newFd);
		//任务结束
#ifdef DEBUG
		printf("-------------------------------------------end\n");
#endif
	}   
}
