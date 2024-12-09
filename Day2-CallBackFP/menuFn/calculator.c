#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function Ponter 
typedef int (*fnPtr)(int, int);

/*
 * @param integer x and y
 * @brief Returns the sum of two integers of type int
 */
int add(int x, int y){
	return x + y;
}

/*
 * @param integer x and y
 * @brief Returns the substraction of two integers of type int
 */
int sub(int x, int y){
	return x - y;
}

/*
 * @param integer x and y
 * @brief Returns the multiplication of two integers of type int
 */
int mul(int x, int y){
	return x * y;
}

/*
 * @param integer x and y
 * @brief Returns the division of two integers of type int
 */
int divison(int x, int y){
	return x/y;
}

/*
 * @param
 * 	x accepts an integer
 * 	y accepts an integer
 * 	callback accepts a funtion pointer of the signature fnPtr
 *
 * @return returns an integer by invoking the function registered with callback.
 */
int callbackFn(int x, int y, fnPtr callback){
	return callback(x, y);
}

int main(int argc, char **argv){
	/* Create a an array of Functions */
	fnPtr fnArr[4] = {add, sub, mul, divison};
	
	/* Enum helps us map the value received from the user to the array index for calling the appropriate function */
	enum arith {ADD, SUB, MUL, DIV};
	enum arith operate;

	/* input from user to perfrom the arithmetic operation on */
	int value, x, y;

	while(1){
		printf("Menu on the table:\n0)Addition\n1)Subtraction\n2)Multiplicaiton\n3)Division\n");
		scanf("%d", &value);
		
		printf("Enter x and y: \n");
		scanf("%d %d", &x, &y);
		
		/* Depending upon the value map to particular switch statement */
		switch(value){
			case ADD:
				printf("Addtion: %d\n", fnArr[ADD](x, y));
				break;
			
			case SUB:
				printf("Subtraction: %d\n", fnArr[SUB](x, y));
				break;
			
			case MUL:
				printf("Multiplication: %d\n", fnArr[MUL](x, y));
				break;

			case DIV:
				printf("Division: %d\n", fnArr[DIV](x, y));
				break;
			default:
				break;
		}

	}	
}
