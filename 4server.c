
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>

// usage of this program:
// ./4server <server-port>

int main(int argc, char *argv[])
{
    int ret = 1;
    if (argc != 2)
    {
        printf("Usage: %s <server-port>\n", argv[0]);
        return ret;
    }

    int port = atoi(argv[1]);

    int sd;
    struct sockaddr_in sin;

    // set port and IP
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);

    // create socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);

    // bind
    ret = bind(sd, (struct sockaddr *)&sin, sizeof(sin));
    if (ret == -1)
    {
        perror("bind");
        return ret;
    }

    char buffer[101];
    if(recvfrom(sd, &buffer, 101, 0, NULL, 0) == -1) {
        perror("recvfrom");
        return ret;
    }

    printf("Received: %s\n", buffer);

    close(sd);
}