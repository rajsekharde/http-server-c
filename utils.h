#ifndef UTILS_H
#define UTILS_H

#define INFO "\033[1;34m[INFO]\033[0m"

void log_request(char *buffer, double elapsed, int resp_c, char *resp_m);

void print_msg(char *msg);

#endif