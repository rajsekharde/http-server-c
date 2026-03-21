# HTTP Server
Fully functional HTTP server built from scratch in C

## Features
- HTTP request parsing
- Routing
- Static file serving
- Concurrent connections
- Logging & Metrics

## Workflow
- Using sys/socket library, create a TCP socket. socket() function returns the file descriptor to the socket.
- Create a sockaddr_in struct with IPv4 address family, accepting connections from "0.0.0.0" and set a port.
- Bind the socket to the address struct.
- Start listening on the socket.

- Accept a connection using accept(). Returns file descriptor to new client socket.
- Use read() to get the request from client fd and store it in a buffer.
- Parse HTTP request, perform business logic, create HTTP response.
- Use write() to copy response to client fd.
- Close the client connection using close().

- Run the steps from accept() to close() inside an infinite loop to keep accepting new connections.

- Using pthread library, create a new thread for each client connection, pass data and detach it. Server can now handle concurrent connections.
- Add mutex locks to avoid race conditions when accessing global variables.

## Project Structure
```bash
server.c -> Starts server, accepts connections and calls parsing, handler functions
request.c -> Functions for parsing and validating HTTP requests
handlers.c -> Handler functions for API endpoints
utils.c -> Logging, misc functions
docs/ -> Notes
static/ -> Static files
*.h -> header files
test.c -> Initial implementation of TCP echo server and minimal HTTP server
```

## Compiling & Running in a Linux machine
```bash
clone git repository

gcc server.c request.c handlers.c utils.c -o server -pthread
./server

OR using script:
chmod +x run.sh
./run.sh

Open localhost:8080 in a browser or using curl

API endpoints:
- GET /
- GET /metrics
```