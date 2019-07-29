#include <stdio.h>

int multi1(int a, int b){
	a = 1;
	int c = a*b;
	int *p;
	*p = 10;
	return c;
}


int main(){
	int a = 10;
	int b = 20;
	int c = multi1(a, b);
	printf("%d * %d = %d\n", a, b, c);
	return 0;
}
