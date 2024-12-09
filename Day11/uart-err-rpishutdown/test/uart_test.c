#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define TARGET_FILE_DIR             "/dev/rpi4_uart_device"

int main(){

    int fd;
    fd = open(TARGET_FILE_DIR, O_RDONLY);

    char buff[128];
    read(fd, buff, sizeof(buff));

    printf("Read: %s\n", buff);
}