#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 4 //B树的阶数

//数据结点
typedef struct data_node{
	int num;
}Data_t, *pData_t;

//B树结点
typedef struct btree_node{
	int dataNum;	//dataNum < M
	struct btree_node *pDad;
	struct btree_node* *pChildArr;	//二维指针
	pData_t *pDataArr;				//二维指针
}BTreeNode_t, *pBTreeNode_t;

typedef struct btree{
	pBTreeNode_t root;
	int m;    //树的阶数
}BTree_t, *pBTree_t;
//m代表树的阶数
pBTreeNode_t createBTreeNode(int m){
	pBTreeNode_t p = (pBTreeNode_t)calloc(1, sizeof(BTreeNode_t));
	p->pChildArr = (pBTreeNode_t*)calloc(m, sizeof(pBTreeNode_t));
	p->pDataArr  = (pData_t*)calloc(m, sizeof(pData_t));
	return p;
}

int BTreeInit(pBTree_t bTree, int m){
	if(NULL==bTree){
		return -1;
	}
	bTree->m = m;
	bTree->root = createBTreeNode(m); 
	return 0;
}

int BTreeInsert(pBTree_t bTree, pData_t pData){
	pBTreeNode_t root = bTree->root;
	int m = bTree->m;
	pData_t *pKey = root->pDataArr;
	pBTreeNode_t *ptr = root->pChildArr;
	if(NULL==root){
		return -1;
	}
	while(root){
		int i, j;
		j = root->dataNum;
		for(i=0; i<j; i++){
			if(pData->num < pKey[i]->num){
				break;
			}
		}
		if(NULL==ptr[i]){
			//插入数据,因为每次插入数据时都在叶子结点，故子结点的指针全为NULL
			for( ; j>=i; j--){
				pKey[j+1] = pKey[j];
			}
			pKey[i] = pData;
			root->dataNum++;
			break;
		}else{
			root = ptr[i];
			pKey = root->pDataArr;
			ptr = root->pChildArr;
			continue;
		}
	}
	while(root->dataNum == m){
		//分裂
		int i, j, m;
		m = bTree->m;
		pBTreeNode_t p;
		pBTreeNode_t pRight = createBTreeNode(m); 
		pBTreeNode_t pLeft = root;
		//分裂后的右子结点
		for(i=0,j=m/2+1; j<=m; i++,j++){
			pRight->dataNum++;
			pRight->pDataArr[i] = root->pDataArr[j];
			pRight->pChildArr[i] = root->pChildArr[j];
		}
		pRight->pChildArr[i] = root->pChildArr[j];
		//分裂后的左子结点
		for(i=m/2; i<=m; i++){
			pLeft->dataNum--;
		}
		//判断当前满结点是否为根结点
		if(NULL==root->pDad){
			p = createBTreeNode(m); 
			//分裂后的中间结点
			p->pDataArr[0] = root->pDataArr[m/2];
			p->dataNum++;
			p->pChildArr[0] = pLeft;
			p->pChildArr[1] = pRight;
			bTree->root = p;
		}else{
			p = root->pDad;
			pData_t pData = root->pDataArr[m/2];
			for(i=0; i<p->dataNum+1; i++){
				if(p->pChildArr[i]==root)
					break;
			}
			j = i;
			for(i=p->dataNum+1; i>=j+1; i--){
				p->pChildArr[i+1] = p->pChildArr[i];
			}
			p->pChildArr[j+1] = pRight;
			for(i=p->dataNum; i>=j; i--){
				p->pDataArr[i+1] = p->pDataArr[i];
			}
			p->pDataArr[i] = pData;
			p->dataNum++;
		}
		pRight->pDad = p;
		pLeft->pDad = p;
	}
	return 0;
}
