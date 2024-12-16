#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PRINT_DEBUG

#define BUFFER_SIZE 1024

/*  Function Pointers   */
int read_from_debugfs(const char *file_name, char *read_buffer, int size);
int write_to_debugfs(const char *file_name);

int main(int argc, char **argv)
{
    /*  Create a buffer */
    char p_buffer[BUFFER_SIZE];
    write_to_debugfs(argv[1]);
    read_from_debugfs(argv[1], p_buffer, BUFFER_SIZE);
}

int read_from_debugfs(const char *file_name, char *read_buffer, int size)
{
    int xRet;

    int debug = open(file_name, O_RDONLY);

#ifdef PRINT_DEBUG
    printf("Debug File Descriptor = %d\n", debug);
#endif

    xRet = read(debug, read_buffer, sizeof(read_buffer));
    if (xRet == -1)
    {
        perror("read");
        return xRet;
    }

    printf("Reading from %s: %s\n", file_name, read_buffer);

    close(debug);

    return 0;
}

int write_to_debugfs(const char *file_name)
{
    int xRet;

    int debugfs = open(file_name, O_WRONLY);
#ifdef PRINT_DEBUG
    printf("Proc File Descriptor = %d\n", debugfs);
#endif

    char value[16];

    printf("Enter the value: ");
    scanf("%s", value);

    int len = strlen(value);

    xRet = write(debugfs, value, len);
    if (xRet == -1)
    {
        perror("write");
        return xRet;
    }

    close(debugfs);

    return 0;
}
