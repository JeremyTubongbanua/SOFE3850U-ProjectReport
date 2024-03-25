#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "pdu.h"

void populate_pdu(pdu *pdu, char type, char *data) {
    memset(pdu, 0, sizeof(struct pdu));
    pdu->type = type;
    strcpy(pdu->data, data);
}

void print_pdu(const pdu *p) {
    printf("PDU: %c %s\n", p->type, p->data);
}

int send_pdu(int sd, pdu *p, struct sockaddr_in *sin, int client) {
    char type = p->type;
    char data[100];
    memset(data, 0, sizeof(char) * 100);
    strcpy(data, p->data);

    char buffer[101];
    memset(buffer, 0, sizeof(char) * 101);
    buffer[0] = type;
    strcpy(buffer + 1, data);

    int valread;
    if(client == 1) {
        valread = sendto(sd, &buffer, 101, 0, NULL, 0);
    } else {
        valread = sendto(sd, &buffer, 101, 0, (struct sockaddr *)sin, sizeof(struct sockaddr_in));
    }

    if(valread == -1) {
        perror("sendto");
        return -1;
    }

    printf("\n\t[SENT] (%d) ", valread);
    print_pdu(p);
    printf("\n");
    return valread;
}

int recv_pdu(int sd, pdu *p, struct sockaddr_in *sin, int client) {
    char buffer[101];
    memset(buffer, 0, sizeof(char) * 101);

    int valread;
    if(client == 1) {
        valread = recvfrom(sd, &buffer, 101, 0, NULL, 0);
    } else {
        socklen_t len = sizeof(struct sockaddr_in);
        valread = recvfrom(sd, &buffer, 101, 0, (struct sockaddr *)sin, &len);
    }
    if(valread == -1) {
        perror("recvfrom");
        return -1;
    }

    populate_pdu(p, buffer[0], buffer + 1);
    printf("\n\t[RECV] (%d) ", valread);
    print_pdu(p);
    return valread;
}