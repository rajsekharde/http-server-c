#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void http_server()
{
    int server_fd, new_socket;
    struct sockaddr_in socket_address;
    int addr_len = sizeof(socket_address);

    char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 20\n\n<h1>Hello World</h1>";

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

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

void tcp_echo_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
    
    listen(server_fd, 5);

    // printf("Socket fd: %d\n", server_fd);
    printf("TCP Echo Server running on port 8080...\n");

    while(1) {
        int client = accept(server_fd, 0, 0);
        char buffer[1024] = {0};

        read(client, buffer, 1024);

        // printf("Client fd: %d\n", client);
        printf("Received: %s\n", buffer);

        send(client, buffer, 1024, 0);

        close(client);
    }
}

int main()
{
    tcp_echo_server();
    return 0;
}
