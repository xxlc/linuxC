#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define SEM_HUGE_2MB 1<<21

int main(){
	int shmid = shmget(1000, SEM_HUGE_2MB, IPC_CREAT|0666|SHM_HUGETLB|SEM_HUGE_2MB);
	if(-1==shmid){
		perror("shmget");
		return -1;
	}
	char *p = (char*)shmat(shmid, NULL, 0);
	if((char*)-1==p){
		perror("shmat");
		return -1;
	}
	if(!fork()){
		strcpy(p, "helloworld");
		exit(0);
	}else{
		wait(NULL);
		p[10] = '\0';
		printf("%s\n", p);
		shmctl(shmid, IPC_RMID, 0);
		return 0;
	}	
	return 0;
}
