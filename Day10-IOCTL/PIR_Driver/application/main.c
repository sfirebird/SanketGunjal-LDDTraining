#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
    int fd;
    
    /*  open the /dev/PIR Device    */
    fd = open("/dev/PIR_Driver", O_RDONLY);

    /*  Read the data   */
    char buff[10];
    read(fd, buff, sizeof(buff));

    printf("reading: %s\n", buff);
}