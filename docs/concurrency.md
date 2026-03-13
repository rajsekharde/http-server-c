# Concurrency

In the basic implementation, an infinite while loop is run and all client connection logic is placed inside the loop. This is a single threaded operation. Each time a client connects, the main thread does not accept new connections till the current client socket is closed.

Solution: Add concurrency using multithreading. For each client connection, a new thread is spawned and all client logic is handled by the thread in the background, while the main thread goes back to listening for new connections.

If multiple threads try to access or modify a shared resource, race condition occurs and wrong values are read/written. Solved by adding mutex (Mutual Exclusion) logic: the resource is locked by a thread, then accessed/modified, then unlocked. Meanwhile, the other threads wait till the resource is unlocked. This way only one thread can access a resource at a time.

Logic:
- Accept client connection and get client socket file descriptor
- Allocate heap memory for client data- fd, metrics, etc
- Create a new thread and pass handle_client() function along with client data
- Detach the thread so it runs in the background
- Main thread starts listening for new connections
- handle_client():
    - store the client data and free its heap memory
    - parse and validate the http request
    - call appropriate handler functions based on the request path
    - close client socket fd
    - update metrics inside mutex-lock logic
    - return (thread stops)

