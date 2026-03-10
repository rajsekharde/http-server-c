# Basic implementation of an http server in C

## Socket Lifecycle:

Socket: A file that represents a network connection. To get a server running, a specific sequence of system calls is followed:
- socket(): Create a new communication endpoint
- bind(): Assign a specific IP and Port to that socket
- listen(): Tell the kernel to wait for incoming connections
- accept(): Grab a connection from the queue
- read() / write(): Exchange data (the HTTP request and response)
- close(): Close the connection


```bash
void http_server()
{
    int server_fd, new_socket;
    struct sockaddr_in socket_address;
    int addr_len = sizeof(socket_address);

    char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 20\n\n<h1>Hello World</h1>";

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Returns the file descriptor to the socket or -1 for failure
    // Domain = AF_INET - Address Family: IPv4
    // Type = SOCK_STREAM - Socket Type: Stream
    // Protocol = 0 - Default Protocol- IPPROTO_TCP

    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = INADDR_ANY;
    socket_address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&socket_address, sizeof(socket_address));

    listen(server_fd, 3);
    
    printf("Server listening on port 8080...\n");

    while (1)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&socket_address, (socklen_t *)&addr_len);
        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);
        printf("Request received:\n%s\n", buffer);

        write(new_socket, response, strlen(response));

        close(new_socket);
    }
}
```

## Initialization
- int server_fd, new_socket;
- struct sockaddr_in socket_address;

- server_fd: The Listening Socket. It's only job is to stay at port 8080 and wait for new connections

- new_socket(): The Connection Socket. Every time a new client connects, the server creates a new socket for that connection

- sockaddr_in: The struct that holds the IP address and Port Number

## Building the socket
- server_fd = socket(AF_INET, SOCK_STREAM, 0);
- socket_address.sin_family = AF_INET;
- socket_address.sin_addr.s_addr = INADDR_ANY;
- socker_address.sin_port = htons(8080);

- socket(): Creates a new socket and returns its file descriptor

- htons(8080): Formats the number 8080 so the network understands it

## Binding & Listening
- bind(server_fd, (struct sockaddr *)&socket_address, sizeof(socket_address));
- listen(server_fd, 3);

- bind(): Attaches the program to the port 8080

- listen(): Tells the kernel to start a queue with a maximum size of 3. 4th person trying to connect will be refused until the previous connections are closed

## Infinite Service Loop
- new_socket = accept(server_fd, (struct sockaddr*)&socket_address, (socketlen_t *)&addr_len);
- Program stops and waits here. It consumes 0% CPU while waiting. When a browser hits localhost:8080, accept wakes up and gives the client the new_socket

- char buffer[1024] = {0};
- read(new_socket, buffer, 1024);
- The buffer fills up the HTTP Request

- write(new_socket, response, strlen(response));
- Send the response in HTTP format

- close(new_socket);
- hanging up the connection


* Double Newline "\n\n" in the response separates the header from the actual content