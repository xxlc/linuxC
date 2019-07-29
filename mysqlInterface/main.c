#include "head.h"
#include <mysql/mysql.h>

int main(int argc, char* argv[]){
	MYSQL myConnection;
	mysql_init(&myConnection);
	if(mysql_real_connect(&myConnection,"127.0.0.1","root","jiawei&i","ems",3306,NULL,0)==NULL){
		printf("mysql_real_connect failed\n");
		return -1;
	}
	printf("connect success\n");

	return 0;
}
