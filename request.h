#ifndef REQUEST_H
#define REQUEST_H

#define MAX_METHOD 8
#define MAX_PATH 32
#define MAX_VERSION 16
#define MAX_HEADERS 10
#define MAX_BODY 512

typedef struct
{
    char *key;
    char *value;
} header_t;

typedef struct
{
    // char method[MAX_METHOD];
    // char path[MAX_PATH];
    // char version[MAX_VERSION];

    char* method;
    char* path;
    char* version;

    header_t* headers;
    int header_count;

    char* body;
} http_request;

int parse_http_request(char *buffer, http_request *req);

int validate_request(http_request *req);

#endif