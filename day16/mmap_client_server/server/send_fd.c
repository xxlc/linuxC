#include "config.h"

int sendFd(int pipeFd, int fd){
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
int recvFd(int pipeFd, int *pfd){
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
