#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

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
    sock_addr.sin_addr.s_addr = INADDR_ANY; // uisten on all available newtwork interfaces
    sock_addr.sin_port = htons(8080);       // specify port number in correct order using htons()
    int addr_len = sizeof(sock_addr);

    // bind socket fd & address structure
    bind(server_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));

    // set up socket for listening to connections & max pending conn queue length to 10
    listen(server_fd, 10);

    // logging
    printf("Server listening on port 8080...\n");

    while (1)
    {
        // blocking call. returns fd for new client conn socket
        int client_fd = accept(server_fd, (struct sockaddr *)&sock_addr, (socklen_t *)&addr_len);

        // store request in buffer
        char buffer[1024] = {0};
        read(client_fd, buffer, 1024);

        // logging
        printf("Request Received: %s\n", buffer);

        // response in http format
        char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 20\n\n<h1>Hello World</h1>";

        // push the response back to client through the socket
        write(client_fd, response, strlen(response));

        // close connection for current client
        close(client_fd);
    }

    return 0;
}
