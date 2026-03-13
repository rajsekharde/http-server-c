# HTTP Server in C
Building a fully functional HTTP server from scratch using C

## Features
- HTTP request parsing
- Routing
- Static file serving
- Concurrent connections
- Logging & Metrics

## Project Structure
```bash
server.c -> Entrypoint. Creates socket connections, calls parsing & handler functions
request.c -> Defines functions for parsing and validating HTTP requests
handlers.c -> Defines handler functions for API endpoints
docs/ -> Notes, Ideas
static/ -> Static files
test.c -> Initial implementation of TCP echo server and minimal HTTP server
```