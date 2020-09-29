#ifndef INIT_HEADER
#define INIT_HEADER

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

// port serwera
extern uint16_t port;
// katalog serwera
extern char* catalog;

// wypisz błąd na stderr i zakończ program
void error_exit(const char* msg);
// wypisz log
void log_msg(char* msg);
// wypisz błąd formatu wejścia i wyjdź
void format_error();
// parsuj argumenty
void parse_args(int argc, char* argv[]);

#endif