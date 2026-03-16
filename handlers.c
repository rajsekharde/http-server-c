#include <stdio.h>  // FILE, fopen()
#include <unistd.h> // write() syscall
#include <string.h> // strlen()

#include "handlers.h"

/*
Functions for handling api endpoints
*/

// handle GET /
int handle_get_index(int client_fd)
{
    // sleep(5); // testing
    // open index.html
    FILE *file = fopen("static/index.html", "r");
    if (file == NULL)
    {
        perror("Failed to open static/index.html");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file); // get file size
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

    while ((bytes = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0)
    {
        write(client_fd, file_buffer, bytes);
    }

    // close file
    fclose(file);

    return 1;
}

// handle GET /metrics
int handle_get_metrics(int client_fd, metrics_struct *metrics)
{
    char body[128];
    sprintf(body, "{\"total_connections\":%d,\"active_connections\":%d}",
            metrics->total_connections,
            metrics->active_connections);

    char header[256];
    sprintf(header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %ld\r\n"
            "\r\n",
            strlen(body));

    write(client_fd, header, strlen(header));
    write(client_fd, body, strlen(body));

    return 1;
}

// handle 404: file not found
int handle_file_not_found(int client_fd)
{
    char header[256];
    char body[256];

    sprintf(body, "<h1>404: File not found</h1>");

    sprintf(header,
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n"
        "\r\n",
        strlen(body));
    
    write(client_fd, header, strlen(header));
    write(client_fd, body, strlen(body));
}