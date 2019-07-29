#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#define COLUMNS_NUM_MAX 10
#define COLUMNS_NAME_MAX 20

int mysqlInit(MYSQL *pConnection, char *user, char *passwd, char *databaseName){
	mysql_init(pConnection);
	if(NULL==mysql_real_connect(pConnection,"127.0.0.1",user,passwd,databaseName,3306,NULL,0)){
		printf("mysqlInit: mysql_real_connect failed\n");
		return -1;
	}
	printf("database connect success\n");
	return 0;
}
/*使用mysql的接口关键点：构建sql语句*/
int mysqlInsert();

int mysqlDelete();

int mysqlQuery(MYSQL *pConnection, char *pColumnName, char *pTableName){
	int ret;
	MYSQL_RES *result;//存放查询结果
	char queryStatement[100];//查询语句
	MYSQL_ROW recordRow;//存储一行数据

	sprintf(queryStatement,"select %s from %s",pColumnName,pTableName);
	ret=mysql_query(pConnection,queryStatement);
	if(ret){
		printf("mysql_query failed\n");
		return -1;
	}

	result=mysql_store_result(pConnection);
	if(result){
		int i, col;
		col = mysql_num_fields(result);
		while( (recordRow=mysql_fetch_row(result)) ){
			for(i=0; i<col; i++){
				printf("%s\t", recordRow[i]);
			}
			printf("\n");
		}
	}
	return 0;
}

int mysqlUpdate();

int mysqlClose();
