#include "config.h"

#define ME 1
#define OTHER 2
#define SIZE_2M 2<<21

int main(int argc, char **argv){
	system("clear");
	//打开一个消息队列
	int msqid = msgget(1000, IPC_CREAT|0600);
	if(-1==msqid){
		perror("msgget");
		return -1;
	}
	//打开一个共享内存，读取消息记录，size大小要与client_scanf相等
	int shmid = shmget(1000, SIZE_2M, IPC_CREAT|0600);
	char *ptr = (char*)shmat(shmid, NULL, 0);

	struct msgQueueNode message;
	int ret;
	char buf[40];
	while(1){
		ret = msgrcv(msqid, &message, sizeof(message), 0, 0);
		if(-1==ret){
			perror("msgrcv");
			return -1;
		}
		bzero(buf, sizeof(buf));
		sprintf(buf, "%s %s", message.msg.usrname, message.msg.time);
		if( message.mtype==ME ){
			printf("%75s\n", buf);
			printf("%75s", message.msg.data);
		}else{
			printf("%-75s\n", buf);
			printf("%-75s", message.msg.data);
		}   
	}
}

