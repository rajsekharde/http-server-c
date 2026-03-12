# Static file serving using a C HTTP server

## Event flow:
- Accept request & get new client socket fd
- Parse HTTP request to get Method, Path, Version- "GET", "/", "HTTP/1.1"
- Map request paths to local file paths- "/" -> "static/index.html"
- Check path and call appropriate handler function
- Open file & get file size in bytes
- Prepare response header & write to client socket fd
- Write file contents to client socket fd
- Close file
- Close client socket fd

## Example: GET / -> Serves index.html
```bash
int client_fd; # client socket file descriptor
char buffer[1024]; # buffer for storing request

read(client_fd, buffer, 1024); # store request in buffer

typedef struct {
    char method[8];
    char path[256];
    char version[16];
} http_request;

http_request req; # struct for storing request data

# parse request and store data in req
sscanf(buffer, "%7s %255s %15s",
    req.method,
    req.path,
    req.version);

if(strcmp(req.path, "/") == 0) # Compare req.path and "/"
{
    FILE* file = fopen("static/index.html", "rb"); # Open index.html
    fseek(file, 0, SEEK_END);
    long size = ftell(file); # Store file size
    rewind(file);

    # Prepare response header & write to client fd
    char header[512];
    sprintf(header,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n"
        "\r\n",
        size);
    write(client_fd, header, strlen(header));

    # Write file contents to client fd
    char file_buffer[1024];
    size_t bytes;
    while((bytes = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0)
    {
        write(client_fd, file_buffer, bytes);
    }

    fclose(file); # Close file
}

close(client_fd); # Close client fd
```