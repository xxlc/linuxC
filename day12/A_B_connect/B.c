#include "config.h"

struct MSG{
	long mtype;
	char mtext[MAX_SIZE];
};

int fd1;
int fd2;
void sigfunc(int signum){
	close(fd1);
	close(fd2);
	printf("pipe closed.\n");
	exit(0);
}

int main(){
    	//收到中断时结束进程
    	signal(SIGINT, sigfunc);
	//打开消息队列
	int msqid = msgget(1000, IPC_CREAT|0644);
	struct MSG msg;
	//打开管道文件准备通信
	int fdw = open("./1", O_WRONLY);
	int fdr = open("./2", O_RDONLY);
	fd1 = fdw;
	fd2 = fdr;
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
			//将消息加入到消息队列
			strcpy(msg.mtext, buf);
			msg.mtype = 1;
			msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
		}
		if( FD_ISSET(STDIN_FILENO, &readfds) ){
			bzero(buf, sizeof(buf));
			read(STDIN_FILENO, buf, sizeof(buf));
			buf[strlen(buf)-1] = '\0';
			write(fdw, buf, strlen(buf));
			system("clear");
			//将消息加入到消息队列
			strcpy(msg.mtext, buf);
			msg.mtype =2;
			msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
		}
	}
}
