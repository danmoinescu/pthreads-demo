#include "pthreads-demo.h"


static void usage(const char *me)
{
    fprintf(stderr, "Usage: %s num_workers work_units\n", me);
}


int main(int argc, const char* const argv[])
{
    if(argc<3)
    {
        usage(argv[0]);
        return 1;
    }
    const int num_workers = atoi(argv[1]);
    const int total_work_units = atoi(argv[2]);
    if(num_workers <= 0 || total_work_units <= 0)
    {
        usage(argv[0]);
        return 1;
    }
    printf("Will process %d unit(s) of work in %d thread(s)\n",
            total_work_units, num_workers);

    pthread_mutex_t worker_send_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t worker_recv_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t worker_recv_cond = PTHREAD_COND_INITIALIZER;

#define WORK_UNIT_BATCH_SIZE 10
    Work work_unit_batch[WORK_UNIT_BATCH_SIZE]; // workers receive work here
    int available_work_units = 0;
    bool is_end_of_work = false;

    //// Start the dispatcher thread
    sem_t dispatcher_ready_sem;
    sem_init(&dispatcher_ready_sem, 0, 0);
    Dispatcher_arg dispatcher_args = {
        .max_work_val = total_work_units,
        .work_units = work_unit_batch,
        .available_work_units = &available_work_units,
        .max_available_work_units = WORK_UNIT_BATCH_SIZE,
        .is_end_of_work = &is_end_of_work,
        .worker_recv_cond = {
            .cond = &worker_recv_cond,
            .mutex = &worker_recv_mutex },
        .ready_sem = &dispatcher_ready_sem,
    };

    // Create a dispatcher thread
    pthread_t dispatcher_thread;
    pthread_create(
            &dispatcher_thread, NULL, dispatcher, &dispatcher_args);
    // Wait for dispatcher thread to become operational
    sem_wait(&dispatcher_ready_sem);
    sem_destroy(&dispatcher_ready_sem);

    //// Create the worker threads
    pthread_t worker_ids[num_workers];
    Worker_arg worker_args[num_workers];
    Result results[total_work_units];
    int result_prev_idx = -1;

    for(int i=0; i<num_workers; ++i)
    {
        // Fill in each worker's arguments (overwrite const members)
        *(int*)&worker_args[i].id = i+1;
        *(Work**)&worker_args[i].work_units = work_unit_batch;
        *(int**)&worker_args[i].available_work_units =
            &available_work_units;
        *(bool**)&worker_args[i].is_end_of_work = &is_end_of_work;
        *(Result**)&worker_args[i].results = results;
        *(int**)&worker_args[i].result_prev_idx = &result_prev_idx;
        *(pthread_mutex_t**)&worker_args[i].worker_send_mutex =
            &worker_send_mutex;

        *(pthread_cond_t**)&worker_args[i].worker_recv_cond.cond =
            dispatcher_args.worker_recv_cond.cond;
        *(pthread_mutex_t**)&worker_args[i].worker_recv_cond.mutex =
            dispatcher_args.worker_recv_cond.mutex;

        // Start the worker thread
        pthread_create(
                &worker_ids[i], NULL, worker, &worker_args[i]);
    }

    // Wait for the workers to finish
    for(int i=0; i<num_workers; ++i)
    {
        pthread_join(worker_ids[i], NULL);
    }
    pthread_cancel(dispatcher_thread);
    pthread_join(dispatcher_thread, NULL);

    // Print the results
    printf("Got %d result(s):", result_prev_idx+1);
    for(int i=0; i<=result_prev_idx; ++i)
    {
        printf(" %d", results[i]);
    }
    printf("\n");
    return 0;
}
