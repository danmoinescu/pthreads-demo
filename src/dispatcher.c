#include "pthreads-demo.h"


void* dispatcher(void *arg)
{
    Dispatcher_arg *p_arg = (Dispatcher_arg*)arg;
    pthread_mutex_lock(p_arg->worker_recv_cond.mutex);

    // Signal the main thread that we're ready
    sem_post(p_arg->ready_sem);

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
                *p_arg->is_end_of_work = true;
                break;
            }

            Work next_work_unit = ++prev_work_val;
            p_arg->work_units[(*p_arg->available_work_units)++] =
                next_work_unit;
        }
        // Notify any waiting workers that a new piece of work is available
        signal_condition(&p_arg->worker_recv_cond);
    }

    // Not reached
    pthread_mutex_unlock(p_arg->worker_recv_cond.mutex);
    return NULL;
}
