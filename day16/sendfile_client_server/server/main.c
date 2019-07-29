#include "config.h"
#define DEBUG

int main(int argc, char* argv[]){
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	char ip[] = "192.168.5.163";
   	unsigned short port = 2000;
	int i;
	int ret = serverTcpInit(&sfd, ip, port);
	if(-1==ret){
		printf("serverTcpInit failed.");
		return -1;
	}
	int proNum = 5;//先每次写死，等代码完整后再改
	proInfo_t *pInfo = (proInfo_t*)calloc(proNum, sizeof(proInfo_t));
	if(NULL==pInfo){
		printf("calloc failed\n");
		return -1;
	}
	ret = create_child_process(pInfo, proNum);
	if(-1==ret){
		printf("create_child_process failed\n");
		return -1;
	}
#ifdef DEBUG
	for(i=0; i<proNum; i++){
		printf("pid=%d, childFd=%d, stats=%d\n", pInfo[i].pid, pInfo[i].childFd,pInfo[i].stats);
	}
#endif
	int epfd = epoll_create(1);
	epollAdd(epfd, sfd);
	for(i=0; i<proNum; i++){
		epollAdd(epfd, pInfo[i].childFd);
	}
	struct epoll_event *events;
	events = (struct epoll_event*)calloc(proNum+1, sizeof(struct epoll_event));
	int n, clientFd;
	char noBusyFlag;
	while(1){
		n = epoll_wait(epfd, events, proNum+1, -1);
		for(i=0; i<n; i++){
			if(events[i].data.fd==sfd){
				//新的client连接
				clientFd = accept(sfd, NULL, NULL);
				for(int j=0; j<proNum; j++){
					if(pInfo[j].stats==LEISURE){
						sendFd(pInfo[j].childFd, clientFd);
						pInfo[j].stats = BUSY;
						printf("%d pid is working\n", pInfo[j].pid);
						close(clientFd);
						break;
					}
				}
				close(clientFd);
			}else{
				for(int j=0; j<proNum; j++){
					if(pInfo[j].childFd==events[i].data.fd){
						read(pInfo[j].childFd, &noBusyFlag, 1);
						pInfo[j].stats = LEISURE;
						break;
					}
				}
			}
		}
	}
	return 0;
}
