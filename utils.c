#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

#include "utils.h"

/*
Misc functions
*/

#define CLEAR "\033[0m"
#define BLACK "\033[1;30m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define GREY "\033[37m"

void log_timestamp()
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);

    printf("[%s]   ", buf);
}

// print request logs
void log_request(http_request* req, int buff_len, double elapsed, int resp_c, char* resp_m)
{
    // size_t len = strlen(buffer);

    // // check if buffer ends with "\r\n\r\n"
    // int needs_crlf = 0;
    // if (len < 4 || strcmp(&buffer[len - 4], "\r\n\r\n") != 0)
    // {
    //     needs_crlf = 1;
    // }

    char* code_col = GREEN;
    if(resp_c != 200)
    {
        code_col = RED;
    }

    log_timestamp();
    printf("%s%d   %s%s   %f s\n", code_col, resp_c, resp_m, CLEAR, elapsed);
    printf("%s", GREY);
    printf("%s %s %s\n", req->method, req->path, req->version); // request line
    printf("Headers: %d\n", req->header_count);
    printf("Body length: %d\n", (int)strlen(req->body));
    printf("Request length: %d\n\n", buff_len);
    printf("%s", CLEAR);

    // printing buffer
    // printf("%s%s%s", GREY, buffer, CLEAR);
    // if (needs_crlf)
    // {
    //     printf("\r\n\r\n");
    // }
}