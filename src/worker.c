#include "pthreads-demo.h"

static void send_local_results(
        Worker_arg *p_arg, Result const * local_results, int how_many);

static bool get_next_work_unit(
        Worker_arg *p_arg, Work *p_work_unit, int *p_misses);


void* worker(void* void_arg)
{
    Worker_arg *p_arg = (Worker_arg*)void_arg;
    struct timespec sleep_spec = { .tv_sec=0, .tv_nsec = 0 };
    /* We'll call a "miss" the situation when a worker thread finds
       the work queue empty and has to sleep until some more work is
       made available. Useful for performance analysis.
    */
    int misses = 0;

#define MAX_LOCAL_RESULTS 10
    Result local_results[MAX_LOCAL_RESULTS];
    int local_results_prev_idx = -1;

    Work work_unit;
    while(get_next_work_unit(p_arg, &work_unit, &misses))
    {
        /* Do some work with the received value (emulate work by
           sleeping 40, 50 or 60 us)
        */
        sleep_spec.tv_nsec = (4 + (work_unit % 3)) * 1000L * 10;
        nanosleep(&sleep_spec, NULL);
        Result result = p_arg->id + work_unit*1000;

        /* Store the result in the local cache. When the local cache is
           full, transfer it into the global location.
        */
        if(++local_results_prev_idx == MAX_LOCAL_RESULTS)
        {
            send_local_results(p_arg, local_results, MAX_LOCAL_RESULTS);
            local_results_prev_idx = 0;
        }
        local_results[local_results_prev_idx] = result;
    }
    // Transfer the remaining results
    send_local_results(p_arg, local_results, local_results_prev_idx+1);
    printf("worker %d done, %d misses\n", p_arg->id, misses);
    return NULL;
}


/* If work is available, place the acquired piece of work in the
   location pointed to by p_work_unit and return true.
   Otherwise return false. */
static bool get_next_work_unit(
        Worker_arg *p_arg, Work *p_work_unit, int *p_misses)
{
    // Acquire a new piece of work
    pthread_mutex_lock(p_arg->worker_recv_cond.mutex);
    while(*p_arg->work_stack_size == 0 && !*p_arg->is_end_of_work)
    {
        // Tell the dispatcher to make another piece of work available
        signal_condition(&p_arg->worker_recv_cond);
        // Wait for the dispatcher to make another piece of work available
        wait_on_condition(&p_arg->worker_recv_cond);
        ++*p_misses;
    }

    bool success; // did we receive any work?
    if(*p_arg->work_stack_size > 0)
    {
        // Get a unit work from the stack
        *p_work_unit = p_arg->work_stack[--*p_arg->work_stack_size];
        // Tell the dispatcher to make another piece of work available
        signal_condition(&p_arg->worker_recv_cond);
        success = true;
    }
    else
    {
        success = false; // no more work
    }
    pthread_mutex_unlock(p_arg->worker_recv_cond.mutex);
    return success;
}


static void send_local_results(
        Worker_arg *p_arg, Result const *local_results, int how_many)
{
        pthread_mutex_lock(p_arg->worker_send_mutex);
        for(int i=0; i<how_many; ++i)
        {
            p_arg->global_results[++(*p_arg->result_prev_idx)] =
                local_results[i];
        }
        pthread_mutex_unlock(p_arg->worker_send_mutex);
}
