#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct{
	int num;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
}Ticket_t;

void* pthread_sales1(void *args){
	Ticket_t* ticket = (Ticket_t*)args;
	while(1){
		pthread_mutex_lock(&ticket->mutex);
		if(ticket->num > 0){
			ticket->num--;
			printf("I am window1, sale success, %d\n", ticket->num);
			if(ticket->num == 0){
				pthread_cond_signal(&ticket->cond);
			}
			pthread_mutex_unlock(&ticket->mutex);
		    	sleep(1);
		}else{
			pthread_mutex_unlock(&ticket->mutex);
			break;
		}
	}
    	pthread_exit(NULL);
}
void* pthread_sales2(void *args){
	Ticket_t* ticket = (Ticket_t*)args;
	while(1){
		pthread_mutex_lock(&ticket->mutex);
		if(ticket->num > 0){
			ticket->num--;
			printf("I am window2, sale success, %d\n", ticket->num);
			if(ticket->num == 0){
				pthread_cond_signal(&ticket->cond);
			}
			pthread_mutex_unlock(&ticket->mutex);
		    	sleep(1);
		}else{
			pthread_mutex_unlock(&ticket->mutex);
			break;
		}
	}
    	pthread_exit(NULL);
}

void* pthread_increase_ticket(void *args){
	Ticket_t* ticket = (Ticket_t*)args;
	pthread_mutex_lock(&ticket->mutex);
	pthread_cond_wait(&ticket->cond, &ticket->mutex);
	ticket->num = 20;
	pthread_mutex_unlock(&ticket->mutex);
    	pthread_exit(NULL);
}

int main(){
	Ticket_t ticket;
	pthread_t pthid1, pthid2, pthid3;
	ticket.num = 20;
	pthread_mutex_init(&ticket.mutex, NULL);
	pthread_cond_init(&ticket.cond, NULL);
	//创建放票子线程
	pthread_create(&pthid3, NULL, pthread_increase_ticket, (void*)&ticket);
	sleep(2);
	//创建卖票子线程
	pthread_create(&pthid1, NULL, pthread_sales1, (void*)&ticket);
	pthread_create(&pthid2, NULL, pthread_sales2, (void*)&ticket);

	pthread_join(pthid1, NULL);
	pthread_join(pthid2, NULL);
	pthread_join(pthid3, NULL);
	return 0;
}
