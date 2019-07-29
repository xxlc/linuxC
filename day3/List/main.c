#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10

typedef struct node{
	int data;
	struct node *next;
}Node_t, *pNode_t;


void ListHeadInsert(pNode_t *ppHead, pNode_t *ppTail, int data){
	pNode_t NewNode = (pNode_t)calloc(1, sizeof(Node_t));
	NewNode->data = data;

    	if( *ppHead == NULL ){
		*ppHead = NewNode;
		*ppTail = NewNode;
	}else{
		NewNode->next = *ppHead;
		*ppHead = NewNode;
	}
}

int main(){
	int data;
	pNode_t phead, ptail;

	phead = ptail = NULL;
	srand( time(NULL) );
	for(int i=0; i<N; i++){
#ifdef DEBUG
		printf("Random number #%d: %d\n", i, rand());
#endif
		data = rand()%1000;
		ListHeadInsert(&phead, &ptail, data);
	}

	pNode_t pCur = phead;
	while( NULL != pCur ){
		printf("%4d", pCur->data);
		pCur = pCur->next;
	}
	printf("\n");
	return 0;
}
