#include <stdio.h>
#include <stdlib.h>
#include "operations.h"

int addFn(int x, int y){
	return x+y;
}

int callbackFn(DOP *dynStr){
	return dynStr->operation(dynStr->var1_int, dynStr->var2_int);
}
