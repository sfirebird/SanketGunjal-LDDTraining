#include <stdio.h>
#include <stdlib.h>

int main(){
	int op;

	printf("Select: 1)APIs 2)SYSCALL\n");
	scanf("%d", &op);
	
	if(op == 1){
		/*	By using libc	*/
		char buff[50];
		printf("Enter the message to log: \n");
		scanf("%[^\n]", buff);

		/* Open a file in write mode */
		FILE *logFile = NULL;
		logFile = fopen("log.txt", "r+");
		if(!logFile){
			perror("fopen");
		}



	}
	else if(op == 2){
		/*	By using syscalls	*/

	}
}
