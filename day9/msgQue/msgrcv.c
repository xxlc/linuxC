#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MSG{
	long mtype;
	char mtext[128];
};

int main(int argc, char* argv[]){
	int msqid = msgget(1000, IPC_CREAT|0600);
	if( -1==msqid ){
		perror("msgget");
		return -1;
	}
	struct MSG buf;
	memset(&buf, 0, sizeof(struct MSG));
	msgrcv(msqid, &buf, sizeof(buf.mtext), atoi(argv[1]), 0);
	printf("%s\n", buf.mtext);	
	msgctl(msqid, IPC_RMID, NULL);
	return 0;
}
