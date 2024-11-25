#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef int (*fnPtr)(int, int);

typedef struct{
	fnPtr cb;
	fnPtr cb1;
	fnPtr cb2;
	fnPtr cb3;
}invoke;

int add(int x, int y){
	return x + y;
}

int sub(int x, int y){
	return x - y;
}

int mul(int x, int y){
	return x * y;
}

int divison(int x, int y){
	return x/y;
}

int callbackFn(int x, int y, fnPtr callback){
	return callback(x, y);
}

int main(int argc, char **argv){
	invoke strPtr = {
		.cb = add,
		.cb1 = sub,
		.cb2 = mul,
		.cb3 = divison
	};

	if(strcmp("add", argv[3]) == 0){
		printf("%d\n", callbackFn(atoi(argv[1]), atoi(argv[2]) , strPtr.cb));
	}
	else if(strcmp("sub", argv[3]) == 0){
		printf("%d\n", callbackFn(atoi(argv[1]), atoi(argv[2]) , strPtr.cb1));
	}
	else if(strcmp("mul", argv[3]) == 0){
		printf("%d\n", callbackFn(atoi(argv[1]), atoi(argv[2]) , strPtr.cb2));
	}
	else if(strcmp("div", argv[3]) == 0){
		printf("%d\n", callbackFn(atoi(argv[1]), atoi(argv[2]) , strPtr.cb3));
	}
	else{
		printf("Usage Error: a.out num1 num2 operation\n");
		return 0;
	}

	return 0;
}
