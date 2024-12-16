#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

//#define FILE_IO
#define PRINT_DEBUG

struct key_value_organized{
    unsigned long long int key;
    char value[128];
};

typedef struct key_value_organized kvg;

/* usage: bin_name file_to_communicate */
int main(int argc, char **argv){

#ifdef FILE_IO
    FILE *pfile = NULL;
#endif

    int xRet = -1;


    if(argc != 2){
        printf("usage error:executable filename\n");
        return 0;
    }

    /*  Assign dynamic memory the __value member    
    kvg pair1 = {
        .value = (char *)malloc(sizeof(char) * 128),
    };
    if(!pair1.value){
        perror("malloc");
        return 0;
    }

    kvg pair2_rx = {
        .value = (char *)malloc(sizeof(char) * 128),
    };
    if(!pair1.value){
        perror("malloc");
        return 0;
    }
    */

    kvg pair1, pair2_rx;


    /*  Get the key value pair from the user and register it into kernel    */
    printf("Enter the key: ");
    scanf("%llu", &pair1.key);

    printf("Enter the value for the key: ");
    scanf("%s", pair1.value);

    printf("Size of pair1: %ld\n", sizeof(pair1));

#ifdef PRINT_DEBUG
    printf("Credentials:\n%llu:%s\n", pair1.key, pair1.value);
#endif

    int len = sizeof(pair1.key) + strlen(pair1.value);

#ifdef PRINT_DEBUG
    printf("length of pair1 = %d\n", len);
#endif
    //int len = sizeof(pair1.key);

#ifdef FILE_IO

    pfile = fopen(argv[1], "r+");
    if(!pfile){
        perror("fopen");
    }
    
    xRet = fwrite(&pair1, sizeof(kvg), len, pfile);
    if(!xRet){
        printf("Bytes written = %d\n", xRet);
        perror("fwrite");
    }

    fclose(pfile);

#else
    /* open the file and get the file descriptor */
    int fd = open(argv[1], O_WRONLY);

    /* write to the char special file mounted at argv[1]*/
    write(fd, &pair1, len);

    close(fd);

    /*  open in read mode only  */
    fd = open(argv[1], O_RDONLY);

    /* read from argv[1] into buff */
    read(fd, &pair2_rx, sizeof(pair2_rx));

    printf("Rx from %s: %lld:%s", argv[1], pair2_rx.key, pair2_rx.value);
    close(fd);
#endif

    return 0;

}
