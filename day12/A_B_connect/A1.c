#include "config.h"

struct message{
	int flag;
	char buf[MAX_SIZE];
};

int main(){
    	//打开共享内存
	int shmid = shmget(1000, sizeof(struct message), IPC_CREAT|0644);
	struct message *pmsg = (struct message*)shmat(shmid, NULL, 0);
	//设置共享内存用信号量，先P(1)后V(0)
	int semid = semget(1000, 2, IPC_CREAT|0644);
	struct sembuf P1;
	struct sembuf V0;
	P1.sem_num = 1;
	P1.sem_op = -1;
	P1.sem_flg = SEM_UNDO;
	V0.sem_num = 0;
	V0.sem_op = 1;
	V0.sem_flg = SEM_UNDO;
	
	struct message msg;
	time_t now;
	char nowtime[30];
	system("clear");
	while(1){
		semop(semid, &P1, 1);
		msg = *pmsg;
		semop(semid, &V0, 1);
		now = time(NULL);
		//bzero(nowtime, sizeof(nowtime));
		strcpy(nowtime, ctime(&now));
		nowtime[strlen(nowtime)-1] = '\0';	
		if( msg.flag==0 ){
			//flag==0即自己发的消息
		    	printf("%65s\n", nowtime);
		    	printf("%65s\n", msg.buf);
		}else{
		    	printf("%-65s\n", nowtime);
			printf("%-65s\n", msg.buf);
		}
	}
}
