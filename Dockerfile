# Stage 1 : Build
FROM gcc:latest AS builder

WORKDIR /app

# Copy all source files, headers
COPY *.c *.h ./
COPY static/ ./static/

# Compile all .c files together
RUN gcc -static server.c utils.c request.c handlers.c threadpool.c taskqueue.c -o server -pthread

# Stage 2 : Creating the lightweight runtime image
FROM alpine:latest

WORKDIR /app

# Copy the compiled server binary from the builder stage
COPY --from=builder /app/server .

# Copy static files
COPY --from=builder /app/static ./static

# Expose server port
EXPOSE 8080

# Run the server when the container starts
CMD ["./server"]