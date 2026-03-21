#include <stdio.h>
#include <string.h>

#include "request.h"

/*
Functions for parsing and validating http requests
*/

typedef struct
{
    char* key;
    char* value;
} header_t;

int parse(char* buffer);

// parse request stored in buffer and store method, path, version in req
int parse_http_request(char *buffer, http_request *req)
{
    // printf("\n%s\n", buffer);
    int parse_res = sscanf(buffer, "%7s %255s %15s",
                           req->method,
                           req->path,
                           req->version
                        );

    parse(buffer);

    return (parse_res == 3); // 1 if sscanf returns 3 (no. of fields), else 0
}

// validate request
int validate_request(http_request *req)
{
    // chech for path traversal attack
    if (strstr(req->path, "..") != NULL)
    {
        return -1;
    }
    return 1;
}

int parse(char* buffer)
{
    char* method = buffer;

    char* path = strchr(method, ' ');
    *path = '\0';
    path += 1;

    char* version = strchr(path, ' ');
    *version = '\0';
    version += 1;

    char* header = strstr(version, "\r\n");
    *header = '\0';
    header += 2;

    header_t headers[10];

    for(int i=0; i<10; i++)
    {
        if(strstr(header, "\r\n") == header) break;
    }

    printf("Method: %s\nPath: %s\nVersion: %s\n\n", method, path, version);

    return 0;
}