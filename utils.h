#ifndef UTILS_H
#define UTILS_H

#define INFO "\033[1;34m[INFO]\033[0m"

#include "request.h"

void log_request(http_request* req, int buff_len, double elapsed, int resp_c, char *resp_m);

#endif