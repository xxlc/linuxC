#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "head.h"
#include "task_que.h"
#include "factory.h"
#include "epoll_op.h"

#define SUCCESS 1
#define FAIL    0

void* pthreadFunc(void*);//线程入口

#endif
