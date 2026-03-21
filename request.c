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
    char* method = buffer; // first char of method
    if(!method) return -1;

    char* path = strchr(method, ' ');
    if (!path) return -1;
    *path = '\0'; // terminate method at first space
    path += 1; // first char of path

    char* version = strchr(path, ' ');
    if (!version) return -1;
    *version = '\0'; // terminate path at 2nd space
    version += 1; // first char of version

    char* header = strstr(version, "\r\n");
    if (!header) return -1;
    *header = '\0'; // terminate version at 1st line break
    header += 2; // headers start

    header_t headers[10];
    int header_count = 0;

    while(header_count <= 10)
    {
        if(strstr(header, "\r\n") == header) break;
        char* key = header;
        // if (!key) return -1;

        char* value = strstr(key, ": ");
        // if (!value) return -1;
        *value = '\0';
        value += 2;

        header = strstr(value, "\r\n");
        if(!header) break;
        *header = '\0';
        header += 2;

        header_t h = {key, value};
        headers[header_count] = h;
        header_count++;
    }

    printf("Method: %s\nPath: %s\nVersion: %s\n\n", method, path, version);
    printf("Headers:\n");
    for(int i=0; i<header_count; i++)
    {
        printf("Key: %s, Value: %s\n", headers[i].key, headers[i].value);
    }

    return 0;
}