#ifndef __MYSQL_OP_H__
#define __MYSQL_OP_H__

#include "head.h"
#include <mysql/mysql.h>

int mysql_initFunc();
int mysql_insertFunc(MYSQL *conn,char* table,char* columns,char* values,char* conditions);
int mysql_deleteFunc();
int mysql_queryFunc();

//用于更新table表中column列名的值，筛选条件为conditions
int updateFunc(MYSQL *conn,char*table,char* column,char* value,char* conditions);


#endif
