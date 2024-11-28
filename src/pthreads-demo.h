#pragma once
#include <pthread.h>
#include <semaphore.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef int Work; // could be any type of data (int[], struct etc)
typedef int Result; // could be any type of data (int[], struct etc)


typedef struct
{
    pthread_cond_t * const cond;
    pthread_mutex_t * const mutex;

} Thread_condition;


typedef struct
{
    const int id;
    const Work * const work_units; // current work units
    int * const available_work_units;
    const bool * const is_end_of_work;
    Result * const results;
    int * const result_prev_idx;
    Thread_condition worker_recv_cond;
    pthread_mutex_t * const worker_send_mutex;
} Worker_arg;


typedef struct
{
    const int max_work_val;
    Work * const work_units; // current work units
    int * const available_work_units;
    const int max_available_work_units;
    bool * const is_end_of_work;
    Thread_condition worker_recv_cond;
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
