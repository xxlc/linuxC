#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
	execl("./add", "add", "3", "4", NULL);
	return 0;
}
