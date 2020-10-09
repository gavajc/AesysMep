#ifndef SOCKNET_H
#define SOCKNET_H
//---------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif
//---------------------------------------------------------------------

void netDisconnect(int socket);
int netIPV4Connect(const char *ip, int type, uint16_t port);
//---------------------------------------------------------------------
#endif
