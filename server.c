#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <signal.h>
#include <sys/time.h>

#include "request.h"
#include "handlers.h"
#include "utils.h"

// listening socket fd
int server_fd;

typedef struct
{
    int client_fd;
    metrics_struct* metrics;
} client_data;

void* handle_client(void* args);

pthread_mutex_t lock;

volatile sig_atomic_t shutdown_requested = 0;

// close server on detecting an interrupt
void handle_signal(int sig)
{
    shutdown_requested = 1;
    close(server_fd);
}

int main()
{
    // register handlers for interrupt signals
    signal(SIGINT, handle_signal); // Ctrl + C
    signal(SIGTERM, handle_signal); // Process kill

    // main listening socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    // change socket option to let the server reuse port 8080 if it's in TIME_WAIT state
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) Failed");
        exit(EXIT_FAILURE);
    }

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
    printf("%s   Server listening on port 8080...\n\n", INFO);

    // initialize metrics
    metrics_struct metrics;
    metrics.total_connections = 0;
    metrics.active_connections = 0;

    // mutex initialization
    pthread_mutex_init(&lock, NULL);

    while (!shutdown_requested)
    {
        // blocking call. returns fd for new client connection socket
        int client_fd = accept(server_fd, (struct sockaddr *)&sock_addr, (socklen_t *)&addr_len);
        if(client_fd < 0)
        {
            if(shutdown_requested) {
                break;
            }
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

    printf("\n%s   Waiting for active connections to finish...\n", INFO);
    usleep(300000); // sleep 0.3 s

    while(metrics.active_connections > 0)
    {
        printf("Active connections: %d\n", metrics.active_connections);
        sleep(1); // sleep 1 s
    }

    // destroy mutex
    pthread_mutex_destroy(&lock);

    printf("%s   Server shut down\n", INFO);

    return 0;
}

// threaded function for handling clients in background
void* handle_client(void* args)
{
    // start timer
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int resp_c = 200;
    char* resp_m = "OK";

    client_data* data = (client_data*)args;

    int client_fd = data->client_fd;
    metrics_struct* metrics = data->metrics;

    // free allocated heap memory for client data
    free(data);

    pthread_mutex_lock(&lock);
    metrics->active_connections += 1;
    metrics->total_connections += 1;
    pthread_mutex_unlock(&lock);

    // store request in buffer
    char buffer[1024] = {0};
    read(client_fd, buffer, 1024);

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
                resp_c = 404;
                resp_m = "Not Found";
            }
        }
        else
        {
            perror("Invalid Request");
            resp_c = 400;
            resp_m = "Bad Request";
        }
    }
    else
    {
        perror("Request Parsing Failed");
        resp_c = 500;
        resp_m = "Internal Server Error";
    }

    // close socket
    close(client_fd);

    // update metrics
    pthread_mutex_lock(&lock);
    metrics->active_connections -= 1;
    pthread_mutex_unlock(&lock);

    // testing
    // sleep(5);

    // stop timer
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    // logging
    log_request(buffer, elapsed, resp_c, resp_m);

    return NULL;
}