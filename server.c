#include "server.h"

// stwórz słuchający serwer tcp
// obsługa błędów
int setup_tcp_server(uint16_t port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error_exit("socket error");
    
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = port;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
        error_exit("bind error");
    if (listen(sockfd, 64) < 0)
        error_exit("listen error");

    return sockfd;
}

// poczekaj nieaktywnie na pakiet od klienta
int receive_packet(int client, char* buffer, size_t buffer_size)
{
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(client, &descriptors);
    
    // czekamy 0.5s na nowy pakiet
    struct timeval tv = {0, 500000};
    int ready = select(client + 1, &descriptors, NULL, NULL, &tv);
    if (ready < 0)
        error_exit("select error");
    if (ready == 0)
        return 0;

    ssize_t bytes = recv(client, buffer, buffer_size, 0);
    if (bytes < 0)
        error_exit("recv error");
    buffer[bytes] = 0;

    return 1;
}

// wyślij n bajtów z bufora "buffer"
void send_all(int sockfd, char* buffer, size_t n)
{
    ssize_t left = n;
    while (left > 0)
    {
        ssize_t sent = send(sockfd, buffer, left, 0);
        if (sent < 0)
            error_exit("send error");
        left -= sent;
        buffer += sent;
    }
}