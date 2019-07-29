#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

void print(const char *dirName, int width){
	DIR *pDir = opendir( dirName );
	struct dirent *fileDirent;
	char path[1024]={0};

	while( fileDirent=readdir(pDir) ){
		if( !strcmp(fileDirent->d_name,".")||!strcmp(fileDirent->d_name,"..") ){
			continue;
		}
		for(int i=0; i<width; i++)
		    printf("-");
		printf("%s\n", fileDirent->d_name);
		if( 4==fileDirent->d_type ){
		    	sprintf(path, "%s%s%s", dirName,"/",fileDirent->d_name);
			print(path, width+4);
		}
	}
	return;
}

int main(int argc, char *args[]){
	
    	print(args[1], 4);
	return 0;

}
