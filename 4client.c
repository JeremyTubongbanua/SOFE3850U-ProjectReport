
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// usage of this program:
// ./4client <server-ip> <server-port> <filename.txt>

typedef struct pdu {
    // D = Data PDU
    // F = Final PDU
    // E = Error PDU
    // C = File PDU
    char type;
    char data[100];
} pdu;

pdu *create_pdu(char type, char *data) {
    pdu *p = (pdu *)malloc(sizeof(pdu));
    p->type = type;
    strcpy(p->data, data);
    return p;
}

int send_pdu(int sd, pdu *p) {
    char type = p->type;
    char data[100];
    strcpy(data, p->data);

    char buffer[101];
    buffer[0] = type;
    strcpy(buffer + 1, data);

    int valread = sendto(sd, &buffer, 101, 0, NULL, 0);
    return valread;
}

pdu *recv_pdu(int sd) {
    char buffer[101];
    int valread = recvfrom(sd, &buffer, 101, 0, NULL, 0);
    if(valread == -1) {
        perror("recvfrom");
        return NULL;
    }

    pdu *p = (pdu *)malloc(sizeof(pdu));
    p->type = buffer[0];
    strcpy(p->data, buffer + 1);
    return p;
}

int main(int argc, char *argv[])
{
    int ret = 1;

    if(argc != 4) {
        printf("Usage: %s <server-ip> <server-port> <filename.txt>\n", argv[0]);
        exit(ret);
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    char *filename = argv[3];

    int sd;
    struct sockaddr_in sin;
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(server_ip);
    ret = connect(sd, (struct sockaddr *)&sin, sizeof(sin));
    if(ret == -1) {
        perror("connect");
        exit(ret);
    } else {
        printf("Connected to server %s:%d\n", server_ip, port);
    }

    pdu *filename_pdu = create_pdu('C', filename);
    ret = send_pdu(sd, filename_pdu);
    if(ret == -1) {
        perror("send_pdu");
        exit(ret);
    } else {
        printf("Sent filename to server\n");
    }

}