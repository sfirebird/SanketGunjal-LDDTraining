#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	FILE *fp = NULL;
	char buff[] = "LinuxDeviceDriver";

	fp = fopen(argv[1], "r+");
	if(fp == NULL){
		perror("fopen");
	}
	
	int len = strlen(buff);
	fwrite(buff, sizeof(*buff), len, fp);
}
