#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student{
	char ID[20];
	char name[30];
	float score;
};

int main(int argc, char* args[]){
	struct student s[3];
	for(int i=0; i<3; i++){	    
		scanf("%s%s%f", s[i].ID, s[i].name, &s[i].score);
	}

	FILE *fp = fopen("./stu_info", "wb+");
	for(int i=0; i<3; i++){
		fprintf(fp, "%10s %10s %3.1f\n", s[i].ID, s[i].name, s[i].score);
	}
	fseek(fp, 0, SEEK_SET);
	for(int i=0; i<3; i++){
		printf("%10s %10s %3.1f\n", s[i].ID, s[i].name, s[i].score);
	}
	return 0;
}
