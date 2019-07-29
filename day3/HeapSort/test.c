#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 10

int compare1(const void *left, const void *right){
	int *p1 = (int*)left;
	int *p2 = (int*)right;
	return *p1 - *p2;
}

void heap_qsort(void *, size_t, size_t, int(*compare)(const void*, const void*));

int main(){
	int data[N];
	
	srand( time(NULL) );
	for(int i=0; i<N; i++){
		data[i] = rand()%1000;
	}
	for(int i=0; i<N; i++){
		printf("%4d", data[i]);
	}
	printf("\n");

	heap_qsort(data, N, sizeof(int), compare1);
	
	for(int i=0; i<N; i++){
		printf("%4d", data[i]);
	}
	printf("\n");
	return 0;
}
