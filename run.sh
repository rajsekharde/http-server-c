#!/usr/bin/env bash

gcc server.c utils.c request.c handlers.c -o server

./server