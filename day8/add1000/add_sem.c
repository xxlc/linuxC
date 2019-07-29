#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>

#define N 10000000

int main(){
    	int semid = semget(1000, 1, IPC_CREAT|0600);
	if( -1==semid ){
		perror("semget");
		return -1;
	}
	semctl(semid, 0, SETVAL, 1);//初始化信号量
	struct sembuf sopp, sopv;
	sopp.sem_num = 0;//代表操作第几把锁，当前只有一把锁，故值为0
	sopp.sem_op = -1;
	sopp.sem_flg = SEM_UNDO;
	sopv.sem_num = 0;
	sopv.sem_op = 1;
	sopv.sem_flg = SEM_UNDO;
	//semop()   用来进行加锁和解锁


	int shmid = shmget(1000, 4<<10, IPC_CREAT|0666);
	int *p = (int*)shmat(shmid, NULL, 0);
	*p = 0;
	if(!fork()){
		for(int i=0; i<N; i++){
		   	//加锁
		    	semop(semid, &sopp, 1);
			*p = *p + 1;
			semop(semid, &sopv, 1);
			//解锁
		}
		exit(0);
	}else{
		for(int i=0; i<N; i++){
		    	semop(semid, &sopp, 1);
			*p = *p + 1;
			semop(semid, &sopv, 1);
		}
		wait(NULL);
		printf("%d\n", *p);
		shmctl(shmid, IPC_RMID, 0);
		return 0;
	}
}
