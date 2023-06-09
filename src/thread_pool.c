//
// Created by Ümit Kara on 9.06.2023.
//

#include <stdlib.h>
#include <stdio.h>
// #include <pthread.h>
#include <windows.h>
#include "thread_pool.h"

typedef struct Task {
    TaskFunction task_function;
    void *arg;
    struct Task *next;
} Task;

struct ThreadPool {
    int thread_count;
    // pthread_t *threads;
    HANDLE *threads;
    Task *task_queue_head;
    // pthread_mutex_t task_queue_mutex;
    // pthread_cond_t task_queue_condition;
    HANDLE task_queue_mutex;
    HANDLE task_queue_condition;
    bool shutdown;
};

static void *thread_pool_worker(void *args) {
    ThreadPool *thread_pool = (ThreadPool *) args;

    while (1) {
        // pthread_mutex_lock(&(pool->task_queue_mutex));
        WaitForSingleObject(thread_pool->task_queue_mutex, INFINITE);

        while (thread_pool->task_queue_head == NULL && !thread_pool->shutdown) {
            // pthread_cond_wait(&(pool->task_queue_condition), &(pool->task_queue_mutex));
            WaitForSingleObject(thread_pool->task_queue_condition, INFINITE);
        }

        if (thread_pool->shutdown) {
            // pthread_mutex_unlock(&(pool->task_queue_mutex));
            ReleaseMutex(thread_pool->task_queue_mutex);
            // pthread_exit(NULL);
            return NULL;
        }

        Task *task = thread_pool->task_queue_head;
        thread_pool->task_queue_head = thread_pool->task_queue_head->next;

        // pthread_mutex_unlock(&(pool->task_queue_mutex));
        ReleaseMutex(thread_pool->task_queue_mutex);

        task->task_function(task->arg);
        free(task);
    }

    return NULL;
}

ThreadPool *thread_pool_create(int thread_count) {
    ThreadPool *thread_pool = (ThreadPool *) malloc(sizeof(ThreadPool));
    if (thread_pool == NULL) {
        fprintf(stderr, "Could not allocate memory for thread pool\n");
        exit(EXIT_FAILURE);
    }

    thread_pool->thread_count = thread_count;
    thread_pool->task_queue_head = NULL;
    thread_pool->shutdown = false;

    thread_pool->threads = (HANDLE *) malloc(sizeof(HANDLE) * thread_count);
    if (thread_pool->threads == NULL) {
        fprintf(stderr, "Could not allocate memory for thread pool threads\n");
        free(thread_pool);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < thread_count; i++) {
        HANDLE thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) thread_pool_worker, thread_pool, 0, NULL);
        if (thread_handle == NULL) {
            fprintf(stderr, "Could not create thread %zu\n", i);
            free(thread_pool->threads);
            free(thread_pool);
            exit(EXIT_FAILURE);
        }
        thread_pool->threads[i] = thread_handle;
    }

    return thread_pool;
}

bool thread_pool_add_task(ThreadPool *pool, TaskFunction function, void *data) {
    Task *new_task = (Task *) malloc(sizeof(Task));
    if (new_task == NULL) {
        fprintf(stderr, "Could not allocate memory for new task\n");
        return false;
    }

    new_task->task_function = function;
    new_task->arg = data;
    new_task->next = NULL;

    // pthread_mutex_lock(&(pool->task_queue_mutex));
    WaitForSingleObject(pool->task_queue_mutex, INFINITE);

    Task *last_task = pool->task_queue_head;
    if (last_task == NULL) {
        pool->task_queue_head = new_task;
    } else {
        while (last_task->next != NULL) {
            last_task = last_task->next;
        }
        last_task->next = new_task;
    }

    // pthread_cond_signal(&(pool->task_queue_cond));
    // pthread_mutex_unlock(&(pool->task_queue_mutex));
    ReleaseMutex(pool->task_queue_mutex);
    return true;
}

void thread_pool_destroy(ThreadPool *pool) {
    if (pool->shutdown || pool == NULL) {
        return;
    }

    for (size_t i = 0; i < pool->thread_count; i++) {
        // pthread_cancel(pool->threads[i]);
        TerminateThread(pool->threads[i], 0);
    }

    // pthread_mutex_destroy(&(pool->task_queue_mutex));
    // pthread_cond_destroy(&(pool->task_queue_cond));
    CloseHandle(pool->task_queue_mutex);
    CloseHandle(pool->task_queue_condition);

    free(pool->threads);

    Task *task = pool->task_queue_head;
    while (task != NULL) {
        Task *next_task = task->next;
        free(task);
        task = next_task;
    }

    free(pool);
}