
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pdu.h"

// usage of this program:
// ./4client <server-ip> <server-port> <filename.txt>
int main(int argc, char *argv[])
{
    int ret = 1;

    if (argc != 4)
    {
        printf("Usage: %s <server-ip> <server-port> <filename.txt>\n", argv[0]);
        exit(ret);
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    char *filename = argv[3];

    int sd;
    struct sockaddr_in sin;

    // create socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    // set port and IP
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(server_ip);

    ret = connect(sd, (struct sockaddr *)&sin, sizeof(sin));
    if (ret == -1)
    {
        perror("connect");
        exit(ret);
    }
    else
    {
        printf("Connected to server %s:%d\n", server_ip, port);
    }

    // Send PDU 'C', 'filename.txt'
    pdu filename_pdu;
    populate_pdu(&filename_pdu, 'C', filename);
    ret = send_pdu(sd, &filename_pdu, &sin, 1);
    if (ret == -1)
    {
        perror("send_pdu");
        exit(ret);
    }
    else
    {
        printf("Sent filename to server\n");
    }

    FILE *fp = fopen("client_received.txt", "w");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }

    pdu response_pdu;

    while (response_pdu.type != 'F' || response_pdu.type != 'E')
    {
        recv_pdu(sd, &response_pdu, &sin, 1);
        printf("Received PDU: %c\n", response_pdu.type);
        switch (response_pdu.type)
        {
            case 'F':
                // received Final PDU
                printf("File received successfully\n");
                fclose(fp);
                break;
            case 'E':
                // received Error PDU
                printf("Error: %s\n", response_pdu.data);
                break;
            case 'D':
                // received Data PDU
                printf("Received data: %s\n", response_pdu.data);
                fprintf(fp, "%s", response_pdu.data);
                memset(&response_pdu, 0, sizeof(response_pdu));
                break;
            default:
                printf("Unknown PDU type: %c\n", response_pdu.type);
                break;
        }
    }
}