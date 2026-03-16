# Using SO_REUSEADDR

## Problem

When the server is running and Ctrl+C is entered, the server sends its final ACK to end a connection and enters a state called TIME_WAIT. By default, the Linux OS keeps the port reserved for 2 x MSL (maximum segment lifetime), typically around 60 s. During this time, the bind() system call fails with the error: Address already in use.

The OS does this to:
- Ensure the last ACK arrived from the client by keeping the connection alive for some time.
- Avoid rogue / late packets from previous connection to get accepted by the new connection, which may lead to data corruption.

## Fix

By setting SO_REUSEADDR, the program tells the kernel to ignore any sockets that are currently in the TIME_WAIT state in the specified port and let the program bind to the socket anyway. 

Linux kernel tracks sequence numbers of packets efficiently. So rogue packets with sequence numbers that don't match the current connection get discarded automatically.

```bash
// Create socket

int opt = 1;
if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt failed");
    exit(EXIT_FAILURE);
}

// Bind socket

# setsockopt() - Used to set options related to a network socket
# setsockopt(socket-fd, option-level, option-name, &option, sizeof(option))
# SOL_SOCKET - Changing a generic socket property
# SO_REUSEADDR - let the server bind to a port if it's in TIME_WAIT state from a previous run
# &opt - Address of option integer. opt = 1 to set this option as Enabled. 0 for disabled
```

## SO_REUSEADDR vs SO_REUSEPORT

SO_REUSEADDR: Allows the server to restart immediately as the old connections are dying

SO_REUSEPORT (Linux 3.9+): Allows multiple separate processes to bind to the same port. The kernel then load-balances the incoming connections across them.