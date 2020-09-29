#include "http.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define RESPONSE_MAX 2048

char response_buffer[RESPONSE_MAX];
size_t response_len;

struct header headers[] = {
    { "GET", offsetof(struct http_request, resource) },
    { "Host", offsetof(struct http_request, host) },
    { "Connection", offsetof(struct http_request, connection) },
};

struct content_type content_types[] = {
    { "txt", "text/plain; charset=utf-8" },
    { "html", "text/html; charset=utf-8" },
    { "css", "text/css; charset=utf-8" },
    { "jpg", "image/jpg" },
    { "jpeg", "image/jpeg" },
    { "png", "image/png" },
    { "pdf", "application/pdf" },
};

// wyzeruj strukture http_request
void http_zero(struct http_request* request)
{
    request->resource = NULL;
    request->host = NULL;
    request->connection = NULL;
}

// ustaw pole struktury request po offsetem "offset" na wartość "val"
void set_field(struct http_request* request, int offset, char* val)
{
    char** it = (char**)((void*)request + offset);
    *it = val;
}

// czy żądanie HTTP jest prawidłowe
int is_valid(struct http_request* request)
{
    return request->resource != NULL &&
        request->host != NULL;
}

// funckja parsuje żądanie HTTP, w bufferze
// na końcu bufora "buffer" powinien być bajt zerowy ('\0')
void parse_http_request(char buffer[], struct http_request* request)
{
    int headers_len = sizeof(headers) / sizeof(struct header);
    const char delims[] = " :\n\r";
    char* line = strtok(buffer, delims);

    while (line != NULL)
    {
        for (int i = 0; i < headers_len; ++i)
        {
            struct header* head = headers + i;
            if (strcmp(head->name, line) == 0)
            {
                line = strtok(NULL, delims);
                if (line != NULL)
                    set_field(request, head->offset, line);
            }
        }
        line = strtok(NULL, delims);    
    }
}

// złóż ścieżkę do pliku: katalog/host/nazwa_pliku
char* get_path(char* catalog, char* host, char* resource)
{
    static char path[200];
    strcpy(path, catalog);
    strcat(path, "/");
    strncat(path, host, 50);
    strcat(path, "/");
    strncat(path, resource, 50);
    return path;
}

// zwraca rozszerzenie pliku lub "" w przypadku braku rozszerzenia
const char* get_extension(const char* file)
{
    const char* dot = strrchr(file, '.');
    if (!dot || dot == file)
        return "";
    return dot + 1;
}

// czy ścieżka wskazuje na katalog
int is_dir(char* path)
{
    DIR* dir = opendir(path);
    if (dir != NULL)
    {
        closedir(dir);
        return 1;
    }
    if (errno == ENOTDIR)
        return 0;
    return -1;
}

// czy żądanie wychodzi poza nasz folder roboczy,
// spoza którego nie możemy nic wysłać
int outside_dir(char* path, char* host)
{
    char resolved[200], dir[200];
    strcpy(dir, catalog);
    strcat(dir, "/");
    strncat(dir, host, 20);
    realpath(dir, resolved);
    // sprawdzamy czy absolutna ścieżka do katalog/host
    // jest prefiksem ścieżki, o którą jest żądanie
    return strncmp(resolved, path, strlen(resolved));
}

// zwraca kod odpowiedzi HTTP na żądanie HTTP
int get_code(struct http_request* request)
{
    if (!is_valid(request))
        return 501;

    char* path = get_path(catalog, request->host, request->resource);
    static char absolute_path[200];
    char* ptr = realpath(path, absolute_path); 

    if (ptr == NULL)
        return 404;
    if (outside_dir(absolute_path, request->host))
        return 403;

    request->resource = absolute_path;
    if (is_dir(absolute_path) > 0)
        return 301;
    return 200;
}

// zwraca wartość pola Content-Type na podstawie rozszerzenia
const char* get_content_type(const char* ext)
{
    int content_type_len = sizeof(content_types) / sizeof(struct content_type);
    for (int i = 0; i < content_type_len; ++i)
    {
        struct content_type* type = content_types + i;
        if (strcmp(type->ext, ext) == 0)
            return type->name;
    }
    return "application/octet-stream";
}

// dodaj nagłówek do odpowiedzi o nazwie "name" i wartości "val" (dodaje \r\n na końcu)
void add_header(const char* name, const char* val)
{
    response_len += sprintf(response_buffer + response_len, "%s %s\r\n", name, val);
}

// zakończ nagłówek HTTP (dodaje pustą linię \r\n)
void end_header()
{
    response_len += sprintf(response_buffer + response_len, "\r\n");
}

// wyślij plik o długości "length" bajtów do klienta
void send_file(int client, FILE* file, long length)
{
    while (length > 0)
    {
        size_t bytes = fread(response_buffer, sizeof(char), MIN(RESPONSE_MAX, length), file);
        send_all(client, response_buffer, bytes);
        length -= bytes;
    }
    fclose(file);
}

// stwórz odpowiedź HTTP na podstawie żądania HTTP i kodu
void send_response(int client, int code, struct http_request* request)
{
    // nagłówek nagłówku HTTP
    switch (code)
    {
        case 200:
            add_header("HTTP/1.1", "200 OK");
            break;
        case 301:
            add_header("HTTP/1.1", "301 Moved Permamently");
            // pole Location
            add_header("Location:", "/index.html");
            break;
        case 403:
            add_header("HTTP/1.1", "403 Forbidden");
            break;
        case 404:
            add_header("HTTP/1.1", "404 Not Found");
            break;
        default:
            add_header("HTTP/1.1", "501 Not Implemented");
    }

    // pole Connection
    if (request->connection == NULL)
        request->connection = "close";
    add_header("Connection:", request->connection); 

    FILE* file = NULL;
    long file_length = 0;

    // pole Content-Type + ustalanie wielkości pliku
    if (code == 200)
    {
        const char* type = get_content_type(get_extension(request->resource));
        add_header("Content-Type:", type);

        file = fopen(request->resource, "rb");
        if (file != NULL)
        {
            fseek(file, 0, SEEK_END);
            file_length = ftell(file);
            if (file_length < 0)
                file_length = 0;
            fseek(file, 0, SEEK_SET);
        }
    }

    // pole Content-Length
    char length_buffer[20];
    sprintf(length_buffer, "%ld", file_length);
    add_header("Content-Length:", length_buffer);
    sprintf(length_buffer, "%ld", file_length);

    end_header();

    // wyślij nagłówek odpowiedzi HTTP
    send_all(client, response_buffer, response_len);
    // wyślij ewentualny plik
    if (file != NULL)
        send_file(client, file, file_length);
}

// stwórz i wyślij odpowiedź HTTP
void send_http_response(int client, struct http_request* request)
{
    response_len = 0;
    int code = get_code(request);

    log_msg("sending response");
    send_response(client, code, request);
}   
