#include "config.h"

struct MSG{
	long mtype;
	char mtext[MAX_SIZE];
};

int main(){
    	//打开消息队列
	int msqid = msgget(1000, IPC_CREAT|0644);
	
	struct MSG msg;
	time_t now;
	char nowtime[30];
	system("clear");
	while(1){
		msgrcv(msqid, &msg, sizeof(msg.mtext), 0, 0);

		now = time(NULL);
		//bzero(nowtime, sizeof(nowtime));
		strcpy(nowtime, ctime(&now));
		nowtime[strlen(nowtime)-1] = '\0';
		if( msg.mtype==2 ){
			//flag==0即自己发的消息
		    	printf("%65s\n", nowtime);
		    	printf("%65s\n", msg.mtext);
		}else{
		    	printf("%-65s\n", nowtime);
			printf("%-65s\n", msg.mtext);
		}
	}
}
