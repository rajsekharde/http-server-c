.PHONY: build run docker-build docker-run

build:
	gcc -g server.c utils.c request.c handlers.c threadpool.c taskqueue.c -o server -pthread

run:
	./server

docker-build:
	docker build -t http-server-c .

docker-run:
	docker run --rm -p 8080:8080 http-server-c