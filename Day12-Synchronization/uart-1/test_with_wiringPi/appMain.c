#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TARGET_FILE_DIR             "/dev/Sanket_uart_driver"

int main(){

    int fd, xRet;

    fd = open(TARGET_FILE_DIR, O_RDWR);
    if(fd < 0){
        perror("open");
        return 0;
    }

    printf("fd = %d\n");
    char wbuff[128], rbuff[128];

    printf("Enter whatever: \n");
    scanf("%s", wbuff);

    int len = strlen(wbuff);
    if((xRet = write(fd, wbuff, len)) < 0){
        perror("write");
        return 0;
    }

    if((xRet = read(fd, rbuff, sizeof(rbuff))) < 0){
        perror("read");
        return 0;
    }
    
    printf("Read: %s\n", rbuff);
}