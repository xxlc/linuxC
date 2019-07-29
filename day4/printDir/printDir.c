#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

int print(const char *dirName, char *fileName){
	DIR *pDir = opendir( dirName );
	if( NULL==pDir ){
		perror("opendir\n");
		return -1;
	}
	struct dirent *fileInfo;
	char path[1024]={0};

	while( (fileInfo=readdir(pDir)) ){
		if( !strcmp(fileInfo->d_name,".")||!strcmp(fileInfo->d_name,"..") ){
			continue;
		}
		if( !strcmp(fileInfo->d_name, fileName) ){
			sprintf(path, "%s%s%s", dirName, "/", fileName);
			printf("%s\n", path);
			return 1;
		}
		if( 4==fileInfo->d_type ){
			sprintf(path, "%s%s%s", dirName, "/", fileInfo->d_name);
		    	int ret = print(path, fileName);
			if( 1==ret )
			    return 1;
		}
	}
	return 0;
}

int main(int argc, char* args[]){
	char *path;

    	chdir(args[1]);
	path = getcwd(NULL, 0);
	//printf("%s", path);
	print(path, args[2]);
	return 0;
}
