#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "threadpool.h"

// Thread function
void *thread_function(void *arg)
{
    Threadpool *t_pool = (Threadpool *)arg;

    while (1)
    {
        // lock mutex
        pthread_mutex_lock(&t_pool->lock);

        while (t_pool->is_active == 1 && t_pool->task_queue == NULL)
        {
            // sleep if pool is active and task queue is empty
            pthread_cond_wait(&t_pool->signal, &t_pool->lock);
        }

        if (!t_pool->is_active && t_pool->task_queue == NULL)
        {
            // exit if pool is shutting down and task queue is empty
            pthread_mutex_unlock(&t_pool->lock);
            break;
        }

        // get first task and pop the queue
        Task *curr_task = t_pool->task_queue;
        if (curr_task)
        {
            pop(&t_pool->task_queue);
        }

        // unlock mutex
        pthread_mutex_unlock(&t_pool->lock);

        if (curr_task)
        {
            // execute the task function
            curr_task->function(curr_task->arg);
            free(curr_task);
        }
    }
    return NULL;
}

// Initialize threadpool
Threadpool *initialize_threadpool(int num_threads)
{
    // allocate memory for struct
    Threadpool *t_pool = (Threadpool *)malloc(sizeof(Threadpool));
    if (!t_pool)
    {
        printf("Failed to allocate memory for Threadpool struct\n");
        return NULL;
    }

    t_pool->num_threads = num_threads;
    t_pool->is_active = 1;
    t_pool->task_queue = NULL; // empty queue

    // intialize mutex
    if (pthread_mutex_init(&t_pool->lock, NULL) != 0)
    {
        printf("Failed to initialize mutex\n");
        free(t_pool);
        return NULL;
    }

    // initialize condition
    if (pthread_cond_init(&t_pool->signal, NULL) != 0)
    {
        printf("Failed to initialize signal\n");
        pthread_mutex_destroy(&t_pool->lock);
        free(t_pool);
        return NULL;
    }

    // allocate memory for pool
    t_pool->pool = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    if (!t_pool->pool)
    {
        printf("Failed to allocate memory for pthread_t array\n");
        pthread_cond_destroy(&t_pool->signal);
        pthread_mutex_destroy(&t_pool->lock);
        free(t_pool);
        return NULL;
    }
    // create threads
    for (int i = 0; i < num_threads; i++)
    {
        if (pthread_create(&t_pool->pool[i], NULL, thread_function, (void *)t_pool) != 0)
        {
            printf("Failed to create thread %d, shutting down pool initialization\n", i);

            // signal shutdown to running threads
            pthread_mutex_lock(&t_pool->lock);
            t_pool->is_active = 0;
            pthread_mutex_unlock(&t_pool->lock);
            pthread_cond_broadcast(&t_pool->signal);

            // join running threads
            for (int j = 0; j < i; j++)
            {
                pthread_join(t_pool->pool[j], NULL);
            }

            free(t_pool->pool);
            pthread_cond_destroy(&t_pool->signal);
            pthread_mutex_destroy(&t_pool->lock);
            free(t_pool);

            return NULL;
        }
    }
    printf("Thread pool initialised & %d threads created\n", num_threads);
    return t_pool;
}

// destroy threadpool
void destroy_threadpool(Threadpool *t_pool)
{
    // mark threadpool as inactive
    pthread_mutex_lock(&t_pool->lock);
    t_pool->is_active = 0;
    pthread_mutex_unlock(&t_pool->lock);

    // wake up all the threads
    pthread_cond_broadcast(&t_pool->signal);

    // join all the threads
    for (int i = 0; i < t_pool->num_threads; i++)
    {
        pthread_join(t_pool->pool[i], NULL);
    }

    // empty task queue
    pthread_mutex_lock(&t_pool->lock);
    while (t_pool->task_queue)
    {
        pop(&t_pool->task_queue);
    }
    pthread_mutex_unlock(&t_pool->lock);

    pthread_cond_destroy(&t_pool->signal);
    pthread_mutex_destroy(&t_pool->lock);

    free(t_pool->pool);
    free(t_pool);

    printf("Threadpool destroyed & memory freed\n");
}

// Add task to queue
void add_task(Threadpool *t_pool, void *(*task_function)(void *), void *arg)
{
    if (!t_pool)
        return;

    pthread_mutex_lock(&t_pool->lock);

    // push task struct to queue
    push(&t_pool->task_queue, task_function, arg);
    // wake up exactly one thread
    pthread_cond_signal(&t_pool->signal);

    pthread_mutex_unlock(&t_pool->lock);

    printf("Task added to queue\n");
}
