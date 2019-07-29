#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(){
	int shmid = shmget(1000, 4<<10, IPC_CREAT|0666);
	if( -1==shmid ){
		perror("shmget");
		return -1;
	}
	char *p = (char*)shmat(shmid, NULL, 0);
	if( (char*)-1==p ){
		perror("shmat");
		return -2;
	}
	strcpy(p, "How are you");	    
	return 0;
}
