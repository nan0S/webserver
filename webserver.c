#include <stdio.h>
#include <unistd.h>

#include "init.h"
#include "server.h"
#include "http.h"

#define BUFFER_SIZE 32000
char recv_buffer[BUFFER_SIZE + 1];

// zajmij się nowym połączeniem
void handle_client(int client)
{
    while (1)
    {
        if (!receive_packet(client, recv_buffer, BUFFER_SIZE))
            break;

        log_msg("received http request");

        struct http_request request;
        http_zero(&request);
        parse_http_request(recv_buffer, &request);

        send_http_response(client, &request);

        if (strcmp(request.connection, "close") == 0)
            break;
    }
    log_msg("closing connection");
    close(client);
}

// uruchom serwer
void run_server(int sockfd)
{
    while (1)
    {
        log_msg("waiting for connection");
        int client = accept(sockfd, NULL, NULL);
        log_msg("established new connection");

        if (client < 0)
            error_exit("accept error");

        handle_client(client);        
        printf("\n");
    }
}

int main(int argc, char* argv[])
{
    parse_args(argc, argv);
    int sockfd = setup_tcp_server(port);
    run_server(sockfd);

    return 0;
}
