#!/usr/bin/env bash

gcc -g server.c utils.c request.c handlers.c -o server -pthread

./server