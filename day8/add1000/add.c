#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 10000000

int main(){
	int shmid = shmget(1000, 4<<10, IPC_CREAT|0666);
	int *p = (int*)shmat(shmid, NULL, 0);
	*p = 0;
	if(!fork()){
		for(int i=0; i<N; i++){
			*p = *p + 1;
		}
		exit(0);
	}else{
		for(int i=0; i<N; i++){
			*p = *p + 1;
		}
		wait(NULL);
		printf("%d\n", *p);
		shmctl(shmid, IPC_RMID, 0);
		return 0;
	}
}
