#include "SocketManager.h"


#define PORT 8080
#define HOST "localhost"

SocketManager::SocketManager() {}

// Taken from https://www.linuxhowtos.org/C_C++/socket.ht
int SocketManager::ConnectSocket() {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
    }

    server = gethostbyname(HOST);

    if (server == NULL) {
        perror("No such host");
        return -1;
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        return -1;
    }

    printf("Connection successful");
    this->m_isock = sockfd;
    return 0;
}

int SocketManager::ReceiveValue(dataObject *a) {
    
    fd_set fds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    
    FD_ZERO(&fds);
    FD_SET(m_isock, &fds);
    select(m_isock+1, &fds, NULL, NULL, &tv);
    
    if (FD_ISSET(m_isock, &fds)) {
        /* The socket_fd has data available to be read */
        m_in = recv(m_isock, m_cbuffer, sizeof(m_cbuffer), 0);

        if (m_in != sizeof(struct dataObject)) {
            return 0;
        }
        struct dataObject data = *((struct dataObject *) m_cbuffer);
        *a = *((struct dataObject *) m_cbuffer);
        return (a->value);
    }
    return -1;
}   

int SocketManager::SendMessage(char* message) {
    return send(m_isock, message, strlen(message), 0);
}

void SocketManager::ClearMessage(dataObject *a) {
    a->value = 0;
}
