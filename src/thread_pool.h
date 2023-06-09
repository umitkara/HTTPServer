//
// Created by Ümit Kara on 9.06.2023.
//

#ifndef HTTPSERVER_THREAD_POOL_H
#define HTTPSERVER_THREAD_POOL_H

#include <stdbool.h>

typedef struct ThreadPool ThreadPool;

// Function pointer type for the tasks that will be added to the thread pool
typedef void (*TaskFunction)(void *arg);

// Creates a new thread pool with the specified number of threads
ThreadPool *thread_pool_create(int thread_count);

bool thread_pool_add_task(ThreadPool *thread_pool, TaskFunction task_function, void *arg);

void thread_pool_destroy(ThreadPool *thread_pool);

#endif //HTTPSERVER_THREAD_POOL_H
