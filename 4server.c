
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "pdu.h"

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

    // create socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    // set port and IP
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sin.sin_port = htons(port);

    // bind
    ret = bind(sd, (struct sockaddr *)&sin, sizeof(sin));
    if (ret == -1)
    {
        perror("bind");
        return ret;
    }

    printf("Server started.\n");

    char buffer[101];
    memset(buffer, 0, sizeof(char) * 101);
    pdu pdu;

    while(1) {

        // receive PDU
        recv_pdu(sd, &pdu, &sin, 0);

        if(pdu.type == 'C') { // file PDU
            // initiate file transfer
            FILE *fp = fopen(pdu.data, "r");
            if(fp == NULL) {
                // send error PDU
                printf("Error opening file: %s\n", pdu.data);
                populate_pdu(&pdu, 'E', "File not found");
                send_pdu(sd, &pdu, &sin, 0);
                continue;
            }
            printf("Successfully opened file: %s\n", pdu.data);

            // begin sending file
            char buffer[100];
            memset(buffer, 0, sizeof(char) * 100);
            while (fgets(buffer, 100, fp) != NULL) {
                // send data PDU
                populate_pdu(&pdu, 'D', buffer);
                ret = send_pdu(sd, &pdu, &sin, 0);
                if(ret == -1) {
                    perror("send_pdu");
                    break;
                }
                memset(buffer, 0, sizeof(char) * 100);
            }
            // send final PDU
            populate_pdu(&pdu, 'F', "File transfer complete");
            send_pdu(sd, &pdu, &sin, 0);
        }
        // teardown
        memset(buffer, 0, sizeof(char) * 101);
    }

    close(sd);
}