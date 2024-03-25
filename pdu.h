#ifndef PDU_H
#define PDU_H

// D = Data PDU
// F = Final PDU
// E = Error PDU
// C = File PDU
typedef struct pdu
{
    char type;
    char data[100];
} pdu;

void populate_pdu(pdu *pdu, char type, char *data);

void print_pdu(const pdu *p);

int send_pdu(int sd, pdu *p, struct sockaddr_in *sin, int client) ;

int recv_pdu(int sd, pdu *p, struct sockaddr_in *sin, int client) ;

#endif