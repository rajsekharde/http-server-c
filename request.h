#ifndef REQUEST_H
#define REQUEST_H

const int MAX_METHOD = 8;
const int MAX_PATH = 256;
const int MAX_VERSION = 16;
const int MAX_METHOD = 8;

typedef struct
{
    char method[8];
    char path[256];
    char version[16];
} http_request;

int parse_http_request(char *buffer, http_request *req);

int validate_request(http_request *req);

#endif