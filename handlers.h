#ifndef HANDLERS_H
#define HANDLERS_H

int handle_get_index(int client_fd);

typedef struct
{
    int total_connections;
    int active_connections;
} metrics_struct;

int handle_get_metrics(int client_fd, metrics_struct *metrics);

#endif