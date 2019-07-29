#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
    	uint16_t hostport = atoi(argv[1]);
	uint16_t netshort = htons(hostport);
    	printf("%04x\n", netshort);
	printf("%04x\n", ntohs(netshort));
	return 0;
}
