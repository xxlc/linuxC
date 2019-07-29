#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>

int main(){
	int semid = semget(1000, 2, IPC_CREAT|0600);
	if( -1==semid ){
		perror("semget");
		return -1;
	}
	//初始化信号量用数组array，10代表仓库位置数目，0代表当前产品数目
	unsigned short array[2] = {10, 0};
	semctl(semid, 0, SETALL, array);
	if( !fork() ){
		//子进程作为生产者
	    	struct sembuf sopp;//用于信号量0
		struct sembuf sopv;//用于信号量1
		sopp.sem_num = 0;
		sopp.sem_op = -1;
		sopp.sem_flg = SEM_UNDO;
		sopv.sem_num = 1;
		sopv.sem_op = 1;
		sopv.sem_flg = SEM_UNDO;
		while(1){
			semop(semid, &sopp, 1);//先执行P(位置数目)
			//生产一个产品,然后执行V(产品数目)
			printf("product success.position num=%d\n", \
				semctl(semid, 0, GETVAL));
			semop(semid, &sopv, 1);
		}
	}else{
		//父进程作为消费者
	    	struct sembuf sopp;//用于信号量1，即产品数
		struct sembuf sopv;//用于信号量0，即位置数
		sopp.sem_num = 1;
		sopp.sem_op = -1;
		sopp.sem_flg = SEM_UNDO;
		sopv.sem_num = 0;
		sopv.sem_op = 1;
		sopv.sem_flg = SEM_UNDO;
		while(1){
			semop(semid, &sopp, 1);//先执行P(产品数目)
			//消费一个产品，然后执行V(位置数目)
			printf("consume success.production num=%d\n", \
				semctl(semid, 1, GETVAL));
			semop(semid, &sopv, 1);
			sleep(1);
		}
	}
}
