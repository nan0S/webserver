#include "init.h"

// port serwera
uint16_t port;
// katalog serwera
char* catalog;

// wypisz błąd na stderr i zakończ program
void error_exit(const char* msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

// wypisz log
void log_msg(char* msg)
{
    printf("[LOG]: %s\n", msg);
}

// wypisz błąd formatu wejścia i wyjdź
void format_error()
{
    fprintf(stderr, "Error parsing command line arguments.\n");
    fprintf(stderr, "Usage: ./webserver port catalog\n");
    exit(EXIT_FAILURE);
}

// parsuj argumenty
void parse_args(int argc, char* argv[])
{
    if (argc != 3)
        format_error();
    port = htons(atoi(argv[1]));
    catalog = argv[2];
}
