#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <sys/wait.h>
#include <fcntl.h>

int send_fd(int pipeFd, int fd){
	struct msghdr msg;
	bzero(&msg, sizeof(msg));
	struct iovec iov[1];
	char buf[10];
	iov[0].iov_base = buf;
	iov[0].iov_len = 10;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	//初始化控制信息
	struct cmsghdr *cmsg;
	int cmsgLen = CMSG_LEN(sizeof(int));//得到包含4个字节数据的控制信息结构体大小
	cmsg = (struct cmsghdr*)calloc(1, cmsgLen);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = cmsgLen;
	*(int*)CMSG_DATA(cmsg) = fd;//在cmsg的type变量后添加数据
	msg.msg_control = cmsg;//将控制信息结构体附在消息的控制变量中
	msg.msg_controllen = cmsgLen;
	int ret = sendmsg(pipeFd, &msg, 0);
	if(-1==ret){
		perror("sendmsg");
		return -1;
	}
	return 0;
}
int recv_fd(int pipeFd, int *pfd){
	struct msghdr msg;
	bzero(&msg, sizeof(msg));
	struct iovec iov[1];
	char buf[10];
	iov[0].iov_base = buf;
	iov[0].iov_len = 10;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	//初始化控制信息
	struct cmsghdr *cmsg;
	int cmsgLen = CMSG_LEN(sizeof(int));//得到包含4个字节数据的控制信息结构体大小
	cmsg = (struct cmsghdr*)calloc(1, cmsgLen);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = cmsgLen;
	msg.msg_control = cmsg;//将控制信息结构体附在消息的控制变量中
	msg.msg_controllen = cmsgLen;
	int ret = recvmsg(pipeFd, &msg, 0);
	if(-1==ret){
		perror("recvmsg");
		return -1;
	}
	*pfd = *(int*)CMSG_DATA(cmsg);//将cmsg中的数据读出来
	return 0;
}

int main(){
	int fds[2];
	socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
	if(!fork()){
		int fd;
		close(fds[0]);
		recv_fd(fds[1], &fd);
		printf("I am child.fd=%d\n", fd);
		printf("I am child.fds[1]=%d\n", fds[1]);
	}else{
		close(fds[1]);
		int fd1 = open("./file1", O_RDWR);
		printf("I am parent.fd1 = %d\n", fd1);
		int fd2 = open("./file2", O_RDWR);
		printf("I am parent.fd2 = %d\n", fd2);
		send_fd(fds[0], fd2);
		wait(NULL);
		return 0;
	}
}
