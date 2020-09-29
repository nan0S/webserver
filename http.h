#ifndef HTTP_HEADER

#define HTTP_HEADER

#include <stdio.h>
#include <stddef.h>
#include <dirent.h>
#include <stdlib.h>

#include "server.h"
#include "init.h"

// struktura trzymjąca informacje o żądaniu HTTP, które przyszło
struct http_request
{
    char* resource;
    char* host;
    char* connection;
};

// nagłówki, którymi jesteśmy zainteresowani (do wyciągnięcia
// z żądania HTTP, które przyszło)
extern struct header 
{
    char* name;
    int offset;
} headers[];

// przypisanie: rozszerzenie pliku -> pole Content-Type
extern struct content_type 
{
    char* ext;
    char* name;
} content_types[];

// bufor na odpowiedź HTTP
extern char response_buffer[];
extern size_t response_len;

// wyzeruj strukture http_request
void http_zero(struct http_request* request);
// ustaw pole struktury request po offsetem "offset" na wartość "val"
void set_field(struct http_request* request, int offset, char* val);
// czy żądanie HTTP jest prawidłowe
int is_valid(struct http_request* request);

// funckja parsuje żądanie HTTP, w bufferze
// na końcu bufora "buffer" powinien być bajt zerowy ('\0')
void parse_http_request(char buffer[], struct http_request* request);

// złóż ścieżkę do pliku: katalog/host/nazwa_pliku
char* get_path(char* catalog, char* host, char* resource);
// zwraca rozszerzenie pliku lub "" w przypadku braku rozszerzenia
const char* get_extension(const char* file);
// czy ścieżka wskazuje na katalog
int is_dir(char* path);
// czy żądanie wychodzi poza nasz folder roboczy,
// spoza którego nie możemy nic wysłać
int outside_dir(char* path, char* host);

// zwraca kod odpowiedzi HTTP na żądanie HTTP
int get_code(struct http_request* request);
// zwraca wartość pola Content-Type na podstawie rozszerzenia
const char* get_content_type(const char* ext);
// dodaj nagłówek do odpowiedzi o nazwie "name" i wartości "val" (dodaje \r\n na końcu)
void add_header(const char* name, const char* val);
// zakończ nagłówek HTTP (dodaje pustą linię \r\n)
void end_header();

// wyślij plik o długości "length" bajtów do klienta
void send_file(int client, FILE* file, long length);
// stwórz odpowiedź HTTP na podstawie żądania HTTP i kodu
void send_response(int client, int code, struct http_request* request);
// stwórz i wyślij odpowiedź HTTP
void send_http_response(int client, struct http_request* request);

#endif
