#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PATH   "../text/moby-dick.txt"
#define ADDR   "127.0.0.1"
#define PORT   1234
#define BUF_SZ 512

int fh, sock;
struct sockaddr_in addr;
const socklen_t ADDRLEN = sizeof(struct sockaddr_in);

static int open_file(char* path)
{
    errno = 0;
    fh = open(path, O_RDONLY);

    if (fh == -1 || errno > 0)
    {
        return 1;
    }

    return 0;
}

static int open_sock(void)
{
    int r;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        printf("Failed to create socket.\n");
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    r = inet_pton(AF_INET, ADDR, &addr.sin_addr);
    if (r < 1)
    {
        printf("Failed to convert address.\n");
        return 1;
    }

    r = bind(sock, (struct sockaddr*)&addr, ADDRLEN);
    if (r < 0)
    {
        printf("Failed to bind.\n");
        return 1;
    }

    return 0;
}

static int tx_file(void)
{
    char* buf;
    ssize_t num_rx, num_tx;
    int brk;

    buf = (char*)malloc(BUF_SZ * sizeof(char));
    if (buf == NULL)
    {
        return 1;
    }

    while (1)
    {
        num_rx = read(fh, (void*)buf, BUF_SZ);
        brk = BUF_SZ - num_rx;
        if (num_rx < 1)
        {
            free(buf);
            return 1;
        }

        num_tx = sendto(sock, buf, num_rx, 0, (struct sockaddr*)&addr, ADDRLEN);
        if (num_tx != num_rx)
        {
            free(buf);
            return 1;
        }

        if (brk != 0)
        {
            break;
        }

    }

    free(buf);
    return 0;
}

int main(void)
{
    if (open_file(PATH) != 0)
    {
        printf("Failed to open file.\n");
        return 1;
    }

    if (open_sock() != 0)
    {
        close(fh);
        return 1;
    }

    if (tx_file() != 0)
    {
        close(fh);
        close(sock);
        printf("File transmission failure.\n");
        return 1;
    }

    return 0;
}

