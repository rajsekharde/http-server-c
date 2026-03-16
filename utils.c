#include <stdio.h>
#include <sys/time.h>

#include "utils.h"

/*
Misc functions
*/

void log_timestamp()
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);

    printf("\033[1;34m[%s]\033[0m   ", buf);
}

void log_request(char* buffer, double elapsed, int resp_c, char* resp_m)
{
    size_t len = strlen(buffer);

    // check if buffer ends with "\r\n\r\n"
    int needs_crlf = 0;
    if (len < 4 || strcmp(&buffer[len - 4], "\r\n\r\n") != 0)
    {
        needs_crlf = 1;
    }

    log_timestamp();
    printf("\033[1;34m%d   %s   %f s\033[0m\n", resp_c, resp_m, elapsed);
    printf("%s", buffer);
    if (needs_crlf)
    {
        printf("\r\n\r\n");
    }
}

// checking module imports
void print_msg(char* msg)
{
    printf("%s message found\n", msg);
}