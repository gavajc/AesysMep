
#include "socknet.h"
//---------------------------------------------------------------------

int netIPV4Connect(const char *ip, int type, uint16_t port)
{
    int socket_desc = -1;
    struct sockaddr_in server;

    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
        int result;
        WSADATA wsaData = {0};

        result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0)
            return -1;
    #endif

    //Create socket
    socket_desc = socket(AF_INET , type , 0);
    if (socket_desc == -1)
        return -1;

    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
        return -1;

    return socket_desc;
}
//---------------------------------------------------------------------

void netDisconnect(int socket)
{
    close(socket);
}
//---------------------------------------------------------------------
