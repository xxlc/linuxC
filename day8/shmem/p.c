#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>

int main(){
	int shmid = shmget(1000, 4<<10, IPC_CREAT|0666);
	char *p = (char *)shmat(shmid, NULL, 0);

	printf("%s\n", p);
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
