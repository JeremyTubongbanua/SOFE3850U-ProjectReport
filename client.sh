#!/bin/bash
rm -f 4client
gcc -o 4client 4client.c
./4client $1 $2 $3
rm -f 4client