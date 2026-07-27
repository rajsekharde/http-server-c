#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "taskqueue.h"
#include <pthread.h>

// Threadpool struct
typedef struct Threadpool
{
    int num_threads;
    int is_active;
    Task *task_queue;
    pthread_t *pool;
    pthread_mutex_t lock;
    pthread_cond_t signal;
} Threadpool;

// Initialize threadpool
Threadpool *initialize_threadpool(int num_threads);

// Destroy threadpool
void destroy_threadpool(Threadpool *t_pool);

// Add task to queue
void add_task(Threadpool *t_pool, void *(*task_function)(void *), void *arg);

#endif