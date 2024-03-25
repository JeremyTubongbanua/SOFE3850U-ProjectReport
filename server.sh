#!/bin/bash
rm -f 4server
gcc -o 4server 4server.c
./4server $1
rm -f 4server