#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void write_to_terminal(const char *message);
void write_to_file(const char *message, char *file_name);

int main(){
    write_to_terminal("Sanket to screen!");
    write_to_file("Sanket to file!", "buff.txt");
}

void write_to_terminal(const char *message){
    int fd_out = 1;
    int len = strlen(message);
    write(fd_out, message, len+1);
}

void write_to_file(const char *message, char *file_name){
    int fd_file = open(file_name, O_CREAT | O_WRONLY | O_RDONLY, 0664);
    int len = strlen(message);
    write(fd_file, message, len);
}



