#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(){
    int fd;
    int checkRet;
    char buff[100];
    
    checkRet = read(0, (char *)buff, 100);
    if(checkRet == -1){
        perror("read");
        return 0;
    }

    fd = open("/dev/simple_driver", O_WRONLY);
    if(fd == -1){
        perror("open");
        return 0;
    }
    int len = strlen(buff);
    checkRet = write(fd, (const char *)buff, len);
    if(checkRet == -1){
        perror("write");
        return 0;
    }

    close(fd);

}
