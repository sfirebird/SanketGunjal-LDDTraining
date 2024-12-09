#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv){
	char buff[] = "FromSysCall";

	int fd = open(argv[1], O_CREAT | O_RDWR);
	int len = strlen(buff);
	write(fd, buff, len);

}
