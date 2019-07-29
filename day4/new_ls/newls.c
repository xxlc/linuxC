#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>


int main(int argc, char* args[]){
	DIR *curPath;
	struct dirent *fileDirent;
	struct stat fileStat;
	char path[1024];
	char time[128]={0};
	char fileType[20];

	curPath = opendir(args[1]);
	while( (fileDirent=readdir(curPath)) ){
		if( !strcmp(fileDirent->d_name,".")||!strcmp(fileDirent->d_name,"..") ){
			continue;	
		}
		sprintf(path, "%s%s%s", args[1],"/",fileDirent->d_name);
		stat(path, &fileStat);

		memcpy(time, ctime(&fileStat.st_mtime)+4, 12);
		time[12] = '\0';

		memset(fileType, 0, sizeof(fileType));
		mode_t temp = fileStat.st_mode;
		int j = temp;
		j = (j>>12)&15;
		switch(j){
			case DT_BLK:strncat(fileType, "b", 1);break;
			case DT_CHR:strncat(fileType, "c", 1);break;
			case DT_DIR:strncat(fileType, "d", 1);break;
			case DT_FIFO:strncat(fileType, "p", 1);break;
			case DT_LNK:strncat(fileType, "l", 1);break;
			case DT_REG:strncat(fileType, "-", 1);break;
			case DT_SOCK:strncat(fileType, "-", 1);break;
			case DT_UNKNOWN:strncat(fileType, "-", 1);break;
			default:printf("switch error.\n");break;
		}
		for(int i=2; i>=0; i--){
			j = temp>>(i*3);
			j = j&07;
			switch(j){
				case 0:strncat(fileType, "---", 3);break;
				case 1:strncat(fileType, "--x", 3);break;
				case 2:strncat(fileType, "-w-", 3);break;
				case 3:strncat(fileType, "-wx", 3);break;
				case 4:strncat(fileType, "r--", 3);break;
				case 5:strncat(fileType, "r-x", 3);break;
				case 6:strncat(fileType, "rw-", 3);break;
				case 7:strncat(fileType, "rwx", 3);break;
				default:printf("switch error.\n");break;
			}
		}
		printf("%s", fileType);
		// 链接数 用户 用户组 文件大小 时间 文件名
		printf(" %ld %s %s %5ld %s %s\n",fileStat.st_nlink,getpwuid(fileStat.st_uid)->pw_name,
				getgrgid(fileStat.st_gid)->gr_name,fileStat.st_size,time,fileDirent->d_name);
	}
	closedir(curPath);
	return 0;
}
