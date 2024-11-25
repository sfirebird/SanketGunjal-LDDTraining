#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operations.h"

#define BUFF_LEN	20

int main(){
	DOP dynamicOp;
	char buff[20];
	int x, y;

	printf("Enter the operation to perform: \n");
	scanf("%s", buff);

	printf("Enter x and y: \n");
	scanf("%d %d", &x, &y);

	if(strncmp("add", buff, 3) == 0){
		dynamicOp.var1_int = x;
		dynamicOp.var2_int = y;
		dynamicOp.operation = &addFn;
		printf("Addition: %d\n", callbackFn(&dynamicOp));
	}

}

