#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>

int main(int argc, char* argv[]){
	struct hostent host;
	struct hostent *p;
	p = gethostbyname(argv[1]);
	if(NULL==p){
		printf("gethostbyname fail.\n");
		return -1;
	}
	host = *p;

	printf("h_name=%s\n", host.h_name);
	for(int i=0; host.h_aliases[i]; i++){
		printf("h_aliases[%d]=%s\n", i, host.h_aliases[i]);
	}
	printf("h_addrtype=%d\n", host.h_addrtype);
	printf("h_lenth=%d\n", host.h_length);
	char hArr[16];
	for(int i=0; host.h_addr_list[i]; i++){
	    	bzero(hArr, sizeof(hArr));
		inet_ntop(host.h_addrtype, host.h_addr_list[i], hArr, sizeof(hArr));
		printf("h_add_list[%d]=%s\n", i, hArr);
	}
	return 0;
}
