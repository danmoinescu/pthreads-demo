#include "pthreads-demo.h"

static void dispatcher_cleanup(void * arg);


void* dispatcher(void *arg)
{
    Dispatcher_arg *p_arg = (Dispatcher_arg*)arg;
    pthread_mutex_lock(p_arg->worker_recv_cond.mutex);
    // Arrange for the mutex to be unlocked when we get cancelled
    pthread_cleanup_push(dispatcher_cleanup, p_arg);

    // Signal the main thread that we're ready
    sem_post(p_arg->ready_sem);

    /* Repeatedly wait for worker threads to sigal us that some work has
       been performed, and replenish the work stack.
    */
    Work prev_work_val = 0;
    while(true)
    {
        wait_on_condition(&p_arg->worker_recv_cond);
        /* Fill in the work_units array */
        while(!*p_arg->is_end_of_work &&
              *p_arg->available_work_units < p_arg->max_available_work_units)
        {
            if(prev_work_val >= p_arg->max_work_val)
            {
                // all work is done
                *p_arg->is_end_of_work = true;
                break;
            }

            // Place a new work unit in the LIFO work stack
            Work next_work_unit = ++prev_work_val;
            p_arg->work_units[(*p_arg->available_work_units)++] =
                next_work_unit;
        }
        // Notify any waiting workers that a new piece of work is available
        signal_condition(&p_arg->worker_recv_cond);
    }

    // Purposefully not reached, so that the main thread can cancel us
    pthread_cleanup_pop(1); // needed to avoid a compilation error
    return NULL;
}


static void dispatcher_cleanup(void * arg)
{
    Dispatcher_arg *p_arg = (Dispatcher_arg*)arg;
    pthread_mutex_unlock(p_arg->worker_recv_cond.mutex);
}
