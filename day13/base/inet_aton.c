#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
	struct in_addr netIP;
	inet_aton(argv[1], &netIP);
	printf("%x\n", netIP.s_addr);
	printf("%x\n", inet_addr(argv[1]));
	return 0;
}
