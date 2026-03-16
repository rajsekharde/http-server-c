Graceful shutdown
- detect an interrupt signal, finish current tasks, close fd's and destroy mutexes
- print shutdown message / logs

Proper logging
- request info: header, body
- timestamp
- response status

Proper HTTP request parsing
- parse headers, body

File serving
- Serve any type of file: css, js, image files
- Stream large files efficiently

Testing
- use Apache Benchmark / wrk to test server performance

Concurrency (serve multiple clients at once)
- implement a thread pool to improve performance
- epoll event loop

HTTP/1.1 Keep-Alive
- allow clients to reuse the same TCP connection for multiple images/scripts

Add a server config file and its parser
- Write server info- port, root directory etc in a config file instead of hard coding
- Build a custom parser in C to read this config file