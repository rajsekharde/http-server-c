#ifndef REQUEST_H
#define REQUEST_H

typedef struct
{
    char method[8];
    char path[256];
    char version[16];
} http_request;

int parse_http_request(char *buffer, http_request *req);

int validate_request(http_request *req);

#endif