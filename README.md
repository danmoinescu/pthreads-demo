# pthreads-demo
This repo is meant to demonstrate the use of various pthreads function calls:
- Thread management: pthread\_create, pthread\_cancel, pthread\_join
- Using mutexes: pthread\_mutex\_lock, pthread\_mutex\_unlock
- Conditions: pthread\_cond\_wait, pthread\_cond\_signal
- Semaphores: sem\_wait, sem\_post

It implements a one-producer, multiple-consumer scenario, with the following components:
- a producer thread (called the dispatcher) places units of work in an array when signalled by any of the worker threads
- a number of worker threads read these work units, perform some work (emulated by sleeping for short periods) on them and place the results in a global data structure
- the main thread sets everything up, waits for the entire work to complete and displays the result


Note that the purpose of this repo is not to provide the most efficient solution to the described scenario but rather, as stated above, to show how the various function calls can be used.
