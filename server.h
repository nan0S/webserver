#ifndef SERVER_HEADER
#define SERVER_HEADER

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "init.h"

// stwórz słuchający serwer tcp, obsługa błędów
int setup_tcp_server(uint16_t port);
// poczekaj nieaktywnie na pakiet od klienta
int receive_packet(int client, char* buffer, size_t buffer_size);
// wyślij n bajtów z bufora "buffer"
void send_all(int sockfd, char* buffer, size_t n);

#endif