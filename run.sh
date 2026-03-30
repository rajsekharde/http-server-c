#!/usr/bin/env bash

# exit immediately if any command returns a non-zero exit code
set -e

gcc -g server.c utils.c request.c handlers.c -o server -pthread

echo -e "\033[37m[$(date '+%F %T')] Compilation complete\033[0m"

echo -e "\033[37m[$(date '+%F %T')] Running server\033[0m"

./server


# (gcc) -pthread -> used for:
#   - linking correctly on all systems
#   - enabling thread-local storage for global variables like errno
#   - ensuring the C library uses thread-safe versions of internal functions

# (echo) -e -> enable interpretation of backslash escapes