#!/usr/bin/env bash

gcc server.c request.c handlers.c -o server

./server