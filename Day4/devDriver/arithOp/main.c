#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define READ_FD     0
#define WRITE_FD    1

#define BUFF_SIZE   100

const char *operation = "1)Double\n2)Square\n";

int main(){
    int retStat = 0;
    int fd;
    int len;    

    /* Open the simple driver */
    fd = open("/dev/simple_driver", O_RDONLY | O_WRONLY);
    if(fd == -1){
        perror("open");
    }

    /*  allocate memory for processing input data   */
    void *buff = malloc(BUFF_SIZE);
    if(buff == NULL){
        perror("malloc");
    }
    
    /*  display the available operation to the user */
    strcpy((char *)buff, (const char *)operation);
    /* Select user operation */
    len = strlen((char *)buff);
    retStat = write(WRITE_FD, (char *)buff, len);
    if(retStat == -1){
        perror("write");
    }

    bzero(buff, BUFF_SIZE);

    /* Take input from user for select operation */
    retStat = read(READ_FD, (char *)buff, BUFF_SIZE);
    if(retStat == -1){
        perror("read");
    }
    
    /*  Depending upon the rx input from the user take the values: */
    int read_op = atoi((char *)buff);
    if(read_op == 1){

        /* take the value for calculating double */
        strcpy((char *)buff, "Enter value for double: ");
        len = strlen(buff);
        retStat = write(WRITE_FD, buff, len);
        if(retStat == -1){
            perror("write");
            return 0;
        }
        bzero(buff, len);
        
        retStat = read(READ_FD, buff, BUFF_SIZE);
        if(retStat == -1){
            perror("read");
            return 0;
        }
        
        /*  pass the buffer to the simple driver    */
        strcat((char *)buff, "double\n");
        
        len = strlen(buff);
        retStat = write(fd, (char *)buff, len);
        if(retStat == -1){
            perror("write");
        }
    }
    else if(read_op == 2){
    
        /* take the value for calculating square */
        strcpy((char *)buff, "Enter value for square: ");
        len = strlen(buff);
        retStat = write(WRITE_FD, buff, len);
        if(retStat == -1){
            perror("write");
            return 0;
        }
        bzero(buff, BUFF_SIZE);
        
        retStat = read(READ_FD, buff, BUFF_SIZE);
        if(retStat == -1){
            perror("read");
            return 0;
        }
        
        strcat((char *)buff, "square");
        
        len = strlen(buff);
        retStat = write(fd, (char *)buff, len);
        if(retStat == -1){
            perror("write");
            return 0;
        }
    }
    else{
        
    }
    
    retStat = close(fd);
    if(retStat == -1){
        perror("close");
    }
   
    bzero(buff, BUFF_SIZE);
    
    FILE *fp = NULL;
    fp = fopen("/dev/simple_driver", "r");
    if(fp == NULL){
        perror("fopen");
    }

    retStat = fread(buff, BUFF_SIZE, sizeof(char), fp);
    if(retStat == 0){
        perror("retstat");
    }

    printf("Data from kernel: %s\n", (char *)buff);

    fclose(fp);
    

    /*  Problem: Not able to read data using read() but when used cat command, 
     *  data is read and displayed
    */
    /* Open the simple driver 
    fd = open("/dev/simple_driver", O_RDONLY | O_WRONLY);
    if(fd == -1){
        perror("open");
    }

    retStat = read(fd, buff, 10);
    if(retStat == 1){
        perror("read");
        return 0;
    }
    printf("reading data: %s\n", (char *)buff);
    */

}

