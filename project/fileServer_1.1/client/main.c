#include "head.h"
#include "tcp_init.h"
#include "epoll_op.h"
#include "transFile.h"
#include "client_function.h"

int main(int argc, char* argv[]){
	if(argc!=3){printf("./client IP PORT\n");return -1;}
	//user prompt
	struct tag_prompt prompt;
	strcpy(prompt.curDir, "/");
	//tcp
	int sfd, ret;
	while(1){
		printf("1. 已有账户，直接登录\n2. 注册新账户\n");
		int type = -1;
		char ch;
		scanf("%d", &type);
		while(scanf("%c",&ch)&&ch!='\n');//清空缓冲
		if(type==1){
			ret=userLogin(&sfd, argv[1], (unsigned short)atoi(argv[2]),prompt.usrName);
			if(0==ret){
				printf("登录成功\n");
				break;
			}else{
				printf("登录失败.用户名或密码错误\n");
				continue;
			}
		}else if(type==2){
			ret=userRegister(&sfd, argv[1], (unsigned short)atoi(argv[2]),prompt.usrName);
			if(0==ret){
				printf("注册成功\n");
				break;
			}else{
				printf("注册失败.网络连接问题或用户名重复\n");
				continue;
			}
		}else{
			printf("无效输入\n");
		}
	}

	//request packet
	struct tag_request req;
	while(1){
		printf("%s@%s >> ", prompt.usrName, prompt.curDir);
		ret = commandScanf(&req, prompt.curDir);
		if(-1==ret){
			printf("command invalid\n");
			continue;
		}
		req.packetLen = sizeof(int) + strlen(req.packetData);
		switch(req.packetType){
			case COMMAND_CD:
				ret = requestCd(sfd, &req, prompt.curDir);break;
			case COMMAND_LS:
				ret = requestLs(sfd, &req);break;
			case COMMAND_PUTS:
				ret = requestPuts(sfd, &req);break;
			case COMMAND_GETS:
				ret = requestGets(sfd, &req);break;
			case COMMAND_REMOVE:break;
			case COMMAND_PWD:
				ret = requestPwd(prompt.curDir);break;
			default:break;	
		}
		if(-1==ret)
			printf("arguments invalid\n");
	}
	return 0;	
}

