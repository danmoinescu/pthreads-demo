#pragma once
#include <pthread.h>
#include <semaphore.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "work.h"

typedef int Result; // could be any type of data (int[], struct etc)


typedef struct
{
    pthread_cond_t * const cond;
    pthread_mutex_t * const mutex;

} Thread_condition;


typedef struct
{
    const int id;
    const Work * const work_stack; // work units available to workers
    int * const work_stack_size;
    const bool * const is_end_of_work; // set by dispatcher to signal the end of all work
    Result * const global_results; // results from all workers
    int * const result_prev_idx; // index in the global_results array
    Thread_condition worker_recv_cond; // dispatcher-worker sync (acquiring work)
    pthread_mutex_t * const worker_send_mutex; // worker sync (saving results)
} Worker_arg;


typedef struct
{
    const int max_work_val; // total amount of work units
    Work * const work_stack; // work units available to workers
    int * const work_stack_size;
    const int max_work_stack_size;
    bool * const is_end_of_work; // signals the end of all work
    Thread_condition worker_recv_cond; // dispatcher-worker sync (acquiring work)
    sem_t * const ready_sem; // tell the main thread that we're ready
} Dispatcher_arg;


void* dispatcher(void *arg);
void* worker(void *arg);


static inline int wait_on_condition(Thread_condition *c)
{
    return pthread_cond_wait(c->cond, c->mutex);
}


static inline int signal_condition(Thread_condition *c)
{
    return pthread_cond_signal(c->cond);
}

