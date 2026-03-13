#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>

#include "request.h"
#include "handlers.h"

typedef struct
{
    int client_fd;
    metrics_struct* metrics;
} client_data;

void* handle_client(void* args);

pthread_mutex_t lock;

int main()
{
    // main listening socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
    {
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    // socket address structure specifically for IPv4
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;         // use IPv4 Address
    sock_addr.sin_addr.s_addr = INADDR_ANY; // listen on all available newtwork interfaces
    sock_addr.sin_port = htons(8080);       // specify port number in correct order using htons()
    int addr_len = sizeof(sock_addr);

    // bind socket fd & address structure
    if(bind(server_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
    {
        perror("Bind Failed");
        exit(EXIT_FAILURE);
    }

    // set up socket for listening to connections & max pending conn queue length to 10
    if(listen(server_fd, 10) < 0)
    {
        perror("Listen Failed");
        exit(EXIT_FAILURE);
    }

    // logging
    printf("Server listening on port 8080...\n");

    // initialize metrics
    metrics_struct metrics;
    metrics.total_connections = 0;
    metrics.active_connections = 0;

    // mutex initialization
    pthread_mutex_init(&lock, NULL);

    while (1)
    {
        // blocking call. returns fd for new client connection socket
        int client_fd = accept(server_fd, (struct sockaddr *)&sock_addr, (socklen_t *)&addr_len);
        if(client_fd < 0)
        {
            perror("Client Socket Failed");
            continue;
        }

        pthread_t thread;

        // allocate heap memory for client data
        client_data* data = (client_data*)malloc(sizeof(client_data));
        if(data == NULL)
        {
            perror("Memory Allocation Failed");
            close(client_fd);
            continue;
        }
        data->client_fd = client_fd;
        data->metrics = &metrics;

        // create a new thread for handling client
        pthread_create(&thread, NULL, handle_client, data);
        
        // detach the thread to run in background
        pthread_detach(thread);
    }

    // destroy mutex
    pthread_mutex_destroy(&lock);

    return 0;
}

// threaded function for handling clients in background
void* handle_client(void* args)
{
    client_data* data = (client_data*)args;

    int client_fd = data->client_fd;
    metrics_struct* metrics = data->metrics;

    // free allocated client data
    free(data);

    pthread_mutex_lock(&lock);
    metrics->active_connections += 1;
    metrics->total_connections += 1;
    pthread_mutex_unlock(&lock);

    // store request in buffer
    char buffer[1024] = {0};
    read(client_fd, buffer, 1024);

    printf("\nRequest Received: %s", buffer); // logging

    http_request req;
    
    if (parse_http_request(buffer, &req) == 1) // parse http request and store in req
    {
        if (validate_request(&req) == 1) // validate request
        {
            if (strcmp(req.path, "/") == 0)
            {
                // handle GET /
                handle_get_index(client_fd);
            }
            else if (strcmp(req.path, "/metrics") == 0)
            {
                // handle GET /metrics
                handle_get_metrics(client_fd, metrics);
            }
            else
            {
                // handle 404 Not found
                handle_file_not_found(client_fd);
            }
        }
        else
        {
            perror("Invalid Request");
        }
    }
    else
    {
        perror("Request Parsing Failed");
    }

    close(client_fd);

    pthread_mutex_lock(&lock);
    metrics->active_connections -= 1;
    pthread_mutex_unlock(&lock);

    return NULL;
}