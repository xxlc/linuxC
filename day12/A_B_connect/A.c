#include "config.h"

struct message{
	int flag;
	char buf[MAX_SIZE];
};

int fd1;
int fd2;
int A1_pid;
void sigfunc(int signum){
	close(fd1);
	close(fd2);
	printf("pipe closed.\n");
	exit(0);
}

int main(){
    	signal(SIGINT, sigfunc);//收到退出信号就执行退出函数
	//打开共享内存
	int shmid = shmget(1000, sizeof(struct message), IPC_CREAT|0644);
	struct message *pmsg = (struct message*)shmat(shmid, NULL, 0);
	//设置共享内存用信号量,先P(0)后V(1)
	int semid = semget(1000, 2, IPC_CREAT|0644);
	unsigned short array[] = {1, 0};
	semctl(semid, 0, SETALL, array);
	struct sembuf P0;
	struct sembuf V1;
	P0.sem_num = 0;
	P0.sem_op = -1;
	P0.sem_flg = SEM_UNDO;
	V1.sem_num = 1;
	V1.sem_op = 1;
	V1.sem_flg = SEM_UNDO;
	//获取A1的pid
	//semop(semid, &P0, 1);
	//A1_pid = pmsg->flag;
	//semop(semid, &V1, 1);
	//打开管道文件，与B通信
	int fdr = open("./1", O_RDONLY);
	int fdw = open("./2", O_WRONLY);
	fd1 = fdr;
	fd2 = fdw;
	system("clear");
	printf("I am ready.\n");
	
	fd_set readfds;
	char buf[MAX_SIZE];
	int ret;
	while(1){
		FD_ZERO(&readfds);
		FD_SET(fdr, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
		select(fdr+1, &readfds, NULL, NULL, NULL);
		if( FD_ISSET(fdr, &readfds) ){
		    	bzero(buf, sizeof(buf));
			ret = read(fdr, buf, sizeof(buf)-1);
			if( 0==ret ){
				kill(getpid(), SIGINT);
			}
			semop(semid, &P0, 1);
			pmsg->flag = 1;
			strcpy(pmsg->buf, buf);
			semop(semid, &V1, 1);	
		}
		if( FD_ISSET(STDIN_FILENO, &readfds) ){
			bzero(buf, sizeof(buf));
			read(STDIN_FILENO, buf, sizeof(buf));
			buf[strlen(buf)-1] = '\0';
			write(fdw, buf, strlen(buf));
			system("clear");
			semop(semid, &P0, 1);
			pmsg->flag = 0;
			strcpy(pmsg->buf, buf);
			semop(semid, &V1, 1);	
		}
	}
}
