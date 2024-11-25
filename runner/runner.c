#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ITER 2493

int main(int argc, char* argv[])
{
    int fd;
    int i;

    fd = open("../text/moby-dick.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("Failed to open file.\n");
        return 1;
    }

    for (i = 0; i < ITER; i++)
    {
        (void)getpid();
        syscall(210, fd);
        (void)getpgrp();
    }

    close(fd);

    return 0;
}

