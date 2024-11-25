#include <stdio.h>
#include <stdlib.h>
#include "myLogger.h"

int main(){
	
	EXTC_t ret = slog(INFO, "Hey");
	
	if(ret == EXIT_SUCC){
		printf("\nWohoo, success!\n");
	}
	else{
		printf("Failure, path to success is near!\n");
	}
}


