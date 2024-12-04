#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

//#define FILE_IO
#define PRINT_DEBUG

struct key_value_organized{
    unsigned long long int key;
    char *value;
};

typedef struct key_value_organized kvg;

/* usage: bin_name file_to_communicate */
int main(int argc, char **argv){

    if(argc != 2){
        printf("usage error:executable filename\n");
        return 0;
    }

    /*  Assign dynamic memory the __value member    */
    kvg pair1 = {
        .value = (char *)malloc(sizeof(char) * 124),
    };

    /*  Get the key value pair from the user and register it into kernel    */
    printf("Enter the key: ");
    scanf("%llu", &pair1.key);

    printf("Enter the value for the key: ");
    scanf("%s", pair1.value);

#ifdef PRINT_DEBUG
    printf("Credentials:\n%llu:%s\n", pair1.key, pair1.value);
#endif

    int len = sizeof(kvg) + strlen(pair1.value);
#ifdef PRINT_DEBUG
    printf("lenght of pair1 = %d\n", len);
#endif
    //int len = sizeof(pair1.key);

#ifdef FILE_IO
    FILE *fp = NULL;    
    fp = fopen("/dev/kv_store", "r+");
    fwrite((char *)KEYVAL, sizeof(char), len, fp);
    fread(buff, 4, 2, fp);
    fclose(fp);
#else
    /* open the file and get the file descriptor */
    int fd = open(argv[1], O_RDONLY | O_WRONLY);

    /* write to the char special file mounted at argv[1]*/
    write(fd, &pair1, len);

    /* read from argv[1] into buff */
    read(fd, &pair1, sizeof(kvg));

    close(fd);
#endif

    return 0;

}
