#ifndef SOCKET_CONNECT_H
#define SOCKET_CONNECT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// struct dataStruc {
//     unsigned int droneID;
//     unsigned int incomingData;
// };

struct dataObject {
    unsigned int value;
};


class SocketManager {
public:

   SocketManager();
   virtual ~SocketManager() {}

    int ConnectSocket();
    int ReceiveValue(dataObject *a);
    int SendMessage(char* message);
    void ClearMessage(dataObject *a);

private:
    int m_isock;
    char m_cbuffer[256];
    int m_in;
};

#endif