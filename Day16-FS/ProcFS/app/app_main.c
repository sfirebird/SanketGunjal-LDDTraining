#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PRINT_DEBUG

#define BUFFER_SIZE 1024

/*  Function Pointers   */
int read_from_proc(const char *file_name, char *read_buffer, int size);
int write_to_proc(const char *file_name);

int main(int argc, char **argv){
    /*  Create a buffer */
    char p_buffer[BUFFER_SIZE];
    write_to_proc(argv[1]);
    read_from_proc(argv[1], p_buffer, BUFFER_SIZE);
}

int read_from_proc(const char *file_name, char *read_buffer, int size){
    int xRet;
    
    int proc_desc = open(file_name, O_RDONLY);

#ifdef PRINT_DEBUG
    printf("Proc File Descriptor = %d\n", proc_desc);
#endif

    xRet = read(proc_desc, read_buffer, size);
    if(xRet == -1){
        perror("read");
        return xRet;
    }

    printf("Reading from %s: %s\n", file_name, read_buffer);

    close(proc_desc);

    return 0;
}

int write_to_proc(const char *file_name){
    int xRet;

    int proc_desc = open(file_name, O_WRONLY);
#ifdef PRINT_DEBUG
    printf("Proc File Descriptor = %d\n", proc_desc);
#endif
    
    char temp_buff[128] = "app_main.c\n\0";
    int len = strlen(temp_buff);

    xRet = write(proc_desc, temp_buff, len);
    if(xRet == -1){
        perror("write");
        return xRet;
    }

    close(proc_desc);

    return 0;
}
