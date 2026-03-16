#include <stdio.h>
#include <string.h>

#include "request.h"

/*
Functions for parsing and validating http requests
*/

// parse request stored in buffer and store method, path, version in req
int parse_http_request(char *buffer, http_request *req)
{
    // printf("\n%s\n", buffer);
    int parse_res = sscanf(buffer, "%7s %255s %15s",
                           req->method,
                           req->path,
                           req->version);

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