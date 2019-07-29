#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <wait.h>

#define N 20000000
typedef struct{
	int val;
	pthread_mutex_t mutex;
}Data_t, *pData_t;

int main(){
	int shmid = shmget(1000, 1<<20, IPC_CREAT|0644);
	if( shmid==-1 ){
		perror("shmget");
		return -1;
	}
	pData_t p = (pData_t)shmat(shmid, NULL, 0);
	if( (void*)-1==p ){
		perror("shmat");
		return -1;
	}
	p->val = 0;
	//初始化锁，设置其共享属性
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&p->mutex, &mattr);
		
    	if( !fork() ){
		//子进程
	    	for(int i=0; i<N; i++){
			pthread_mutex_lock(&p->mutex);
			p->val++;
			pthread_mutex_unlock(&p->mutex);
		}
	}else{
	    	//父进程
	    	for(int i=0; i<N; i++){
			pthread_mutex_lock(&p->mutex);
			p->val++;
			pthread_mutex_unlock(&p->mutex);
		}
		wait(NULL);
		printf("%d\n", p->val);
		return 0;
	}
}
