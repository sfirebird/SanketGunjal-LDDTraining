#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "myLogger.h"

EXTC_t slog(PLEVEL_t logLevel, const char *message){

	EXTC_t retStat = EXIT_SUCC;
	
	/* allocate dynamic memory */
	char *buff = NULL;
	buff = (char *)malloc(100 * sizeof(char));
	if(buff == NULL){
		printf("in buff");
		perror("malloc");
		return EXIT_FAIL;
	}
	
	/* Check the message to be not NULL*/
	if(message == NULL){
		printf("In message\n");		
		return EXIT_FAIL;
	}

	/* Check the log level to be correct */
	if(logLevel != INFO && logLevel != WARN && logLevel != ERR){
		printf("%d", logLevel);
		printf("log level\n");
		return EXIT_FAIL;
	}

	/* Open a log file, if is not found create a log_file.txt 
	 * *** Need to include the error checking for open()
	 */
	int fd;
	if((fd = open("log.txt", O_CREAT | O_APPEND | O_WRONLY, 0664)) == -1){
		perror("open");
		return EXIT_FAIL;
	}
	printf("fd = %d\n", fd);
	
	/* Write the date to the file */
	buff = (char *)__DATE__;
	//buff_len = strlen(buff);
	if(write(fd, buff, (sizeof(buff) + 1)) == -1){
		perror("write");
		return EXIT_FAIL;
	}

	if(close(fd) == -1){
		perror("close");
		return EXIT_FAIL;
	}

	return EXIT_SUCC;
}
