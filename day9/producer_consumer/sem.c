#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int main(){
	int semid = semget(1000, 1, IPC_CREAT|0600);
	if( -1==semid ){
		perror("semget");
		return -1;
	}
	union semun init_arg;//设置信号量初始化用参数
	init_arg.val = 0;
	semctl(semid, 0, SETALL, init_arg);//对信号量进行初始化	
	if( !fork() ){
		struct sembuf sem;
		memset(&sem, 0, sizeof(struct sembuf));
		sem.sem_num = 0;
		sem.sem_op = 1;
		sem.sem_flg = SEM_UNDO;
		while(1){
			semop(semid, &sem, 1);
			printf("productor total number:%d\n", semctl(semid, 0, GETVAL));
			sleep(1);
		}
	}else{
		sleep(2);
		struct sembuf sem;
		memset(&sem, 0, sizeof(struct sembuf));
		sem.sem_num = 0;
		sem.sem_op = -1;
		sem.sem_flg = SEM_UNDO;
		while(1){
			semop(semid, &sem, 1);
			printf("consumer total number:%d\n", semctl(semid, 0, GETVAL));
			sleep(2);
		}
	}
}
