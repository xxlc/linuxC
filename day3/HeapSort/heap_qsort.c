//#include "heap_qsort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void max_heap(char *arr, int start, int end, int size, int (*compare)(const void*, const void*)){
	int dad = start;
	int son = start*2+1;
	char *temp = (char*)malloc(size);

	while( son<end ){
		if( son+1<end && compare(arr+size*son, arr+size*(son+1))<0 ){
			son = son+1;
		}
		if( compare(arr+size*dad, arr+size*son)<0 ){
			memcpy(temp, arr+size*dad, size);
			memcpy(arr+size*dad, arr+size*son, size);
			memcpy(arr+size*son, temp, size);
			dad = son;
			son = dad*2+1;
		}else{
		    	free(temp);
			return;
		}
	}
	free(temp);
}

void heap_qsort(void *buf, size_t num, size_t size, int (*compare)(const void*, const void*)){
	char *arr = (char*)buf;
	char *temp = (char*)malloc(size);

	for(int i=(int)num/2-1; i>=0; i--){
		max_heap(arr, i, (int)num, (int)size, compare);
	}

	for(int i=num-1; i>0; i--){
		memcpy(temp, arr, size);
		memcpy(arr, arr+size*i, size);
		memcpy(arr+size*i, temp, size);

		max_heap(arr, 0, i, (int)size, compare);
	}
	free(temp);
}
