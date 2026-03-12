#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

typedef struct {
    char method[8];
    char path[256];
    char version[16];
} http_request;

int parse_http_request(char* buffer, http_request* request);

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

    while (1)
    {
        // blocking call. returns fd for new client conn socket
        int client_fd = accept(server_fd, (struct sockaddr *)&sock_addr, (socklen_t *)&addr_len);
        if(client_fd < 0)
        {
            perror("Client Socket Failed");
            continue;
        }

        // store request in buffer
        char buffer[1024] = {0};
        read(client_fd, buffer, 1024);

        printf("\nRequest Received: %s", buffer); // logging

        http_request req;
        parse_http_request(buffer, &req); // parse http request and store in req
        // printf("Method: %s, Path: %s, Version: %s\n", req.method, req.path, req.version);

        // response in http format
        char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 20\n\n<h1>Hello World</h1>";

        // push the response back to client through the socket
        write(client_fd, response, strlen(response));

        // close connection for current client
        close(client_fd);
    }

    return 0;
}

// Parse HTTP request to get Method, Path, Version and store in an http_request struct
int parse_http_request(char* buffer, http_request* request)
{
    return sscanf(buffer, "%7s %255s %15s",
        request->method,
        request->path,
        request->version);
}