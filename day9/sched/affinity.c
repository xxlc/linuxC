#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>

int main(){
	cpu_set_t set;
	CPU_ZERO(&set);
	int ret = sched_getaffinity(0, sizeof(cpu_set_t), &set);
	if( -1==ret ){
		perror("sched_getaffinity\n");
		return -1;
	}
	for(int i=0; i<__CPU_SETSIZE; i++){
		int cpu;
		cpu = CPU_ISSET(i, &set);
		printf("cpu=%i is %s\n", i, cpu?"set":"unset");
	}
	return 0;
}
