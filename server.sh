#!/bin/bash
rm -f 4server
gcc -o 4server 4server.c pdu.c -Ipdu.h
./4server $1
rm -f 4server