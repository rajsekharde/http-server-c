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

int handle_get_index();

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
        if(parse_http_request(buffer, &req) != 3) // parse http request and store in req
        {
            perror("Request Parsing Failed");
            continue;
        }
        // printf("Method: %s, Path: %s, Version: %s\n", req.method, req.path, req.version);

        // check for path traversal attack
        if (strstr(req.path, "..") != NULL)
        {
            perror("Invalid Request");
            continue;
        }

        // handle GET "/"
        if(strcmp(req.path, "/") == 0)
        {
            handle_get_index(client_fd);
            close(client_fd);
        }

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
    // return number of variables parsed. success = 3
    return sscanf(buffer, "%7s %255s %15s",
        request->method,
        request->path,
        request->version);
}

// Handler for GET "/"
int handle_get_index(int client_fd)
{
    FILE* file = fopen("static/index.html", "rb");
    if(file == NULL) {
        perror("File Open Failed");
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    // prepare header
    char header[512];
    sprintf(header,
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: %ld\r\n"
    "\r\n",
    size);
    // write header to client socket
    write(client_fd, header, strlen(header));

    // write file contents to client socket
    char file_buffer[1024];
    size_t bytes;

    while((bytes = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0)
    {
        write(client_fd, file_buffer, bytes);
    }

    // close file
    fclose(file);

    return 1;
}