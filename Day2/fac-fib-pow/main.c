#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ARR_FN_LEN		3U
#define BUFF_LEN		20U

typedef int (*fnPtr)(int);

// Function Prototypes
int factorial(int num);
void fib(void);
void power(void);

int main(){
	
	/* hold the address of functions */
	fnPtr arrFn[3] = {factorial};
	
	char buff[BUFF_LEN];
	int userIp;
	
	printf("Enter the operation: \n");
	scanf("%s", buff);
	
	/* compare the string from user	*/
	if(strcmp("factorial", buff) == 0){
		printf("Enter the number to get factorial: \n");
		scanf("%d", &userIp);
		printf("Factorial of %d = %d\n", userIp, arrFn[0](userIp));
			
	}
	else if(strcmp("Fibonacci", buff) == 0){
			
	}
	else if(strcmp("power", buff) == 0){
	
	}
	else{
		printf("Usage error!\n");
		return 0;
	}

	return 0;
}

int factorial(int num){
	static int result = 1;
	if(num){
		result = num * result;
		factorial(--num);
	}else{
		return result;
	}

	return result;
}

void fib(void){

}

void power(void){

}

