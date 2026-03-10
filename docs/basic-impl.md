# Basic implementation of an http server in C

## Socket Lifecycle:

Socket: A file that represents a network connection. To get a server running, a specific sequence of system calls is followed:
- socket(): Create a new communication endpoint
- bind(): Assign a specific IP and Port to that socket
- listen(): Tell the kernel to wait for incoming connections
- accept(): Grab a connection from the queue
- read() / write(): Exchange data (the HTTP request and response)
- close(): Close the connection

## Basic TCP Echo Server
```bash
void tcp_echo_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
    listen(server_fd, 5);

    printf("TCP Echo Server running on port 8080...\n");

    while(1) {
        int client = accept(server_fd, 0, 0);
        char buffer[1024] = {0};

        read(client, buffer, 1024);
        printf("Received: %s\n", buffer);

        send(client, buffer, 1024, 0);

        close(client);
    }
}
```
### Socket creation:
- int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

- AF_INET : "Address Family: IPv4". Tells the kernel we want to use standard IP addresses like 127.0.0.1

- SOCK_STREAM: Tells the kernel we want TCP- a connection-oriented protocol. It requires a connection established before talking and guarantees that data arrives in the correct order without corruption

- Return Value: socket() returns an integer- a File Descriptor for the socket

### Address Structure:
- struct sockaddr_in addr = {AF_INET, htons(8080), {INADDR_ANY}};

- htons(8080): Host To Network Short - ensures the number 8080 is flipped correctly so the router understands it

- INDRR_ANY: Constat that tells the server to bind to all available IP addresses on the machine (Localhost, WiFi, Ethernet)

### Binding and Listening:
- bind(server_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
- listen(server_fd, 5);

- bind(): Attaches the program to the specific port (8080). Fails if another program is already using this port

- listen(): Tells the kernel to start queueing up incoming connections. The number 5 is the "backlog"- it's the maximum number of people who can be "on hold" before the server starts rejecting them

### The Infinite Loop:
- while(1) {}

- int client = accept(server_fd, 0, 0);
- accept(): This is a blocking call. The program will pause here and do nothing until someone tries to connect to port 8080. When they do, accept returns a new file descriptor (client) specifically for that person

- read(client, buffer, 1024): This pulls bytes out of the network pipe and puts them into the buffer(a simple array of characters)

- send(client, buffer, 1024), 0): This pushes those same bytes back through the pipe to the client

