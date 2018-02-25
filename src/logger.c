#include "logger.h"

/*
 * Notes from arch diagram
 *
 * Initialization of primary logging thread in main Thread-local
 * storage of logging queue in each thread Each logging queue is
 * registered with main logger (either through global logging
 * pointer, or logger pointer passed into each thread).  Logging
 * posts by thread should not be blocking. Consuming log messages
 * from thread's queue handled by logger thread.  Use single
 * condition variable for notifying logger upon adding records. Will
 * cause minimal blocking if multiple threads attempt to signal
 * simultaniously.
 *
 */

// Todo create single_ended_queue

#define MAX_QUEUES 256

typedef logger_struct
{
    single_ended_queue* queues[MAX_QUEUES];
    FILE *fp;
    // rotation settings
} logger_struct;


// Called by main?
void initialize_logger(logger_struct *ls)
{
}

void add_thread(logger_struct *ls)
{
}

// Called by some timer system,
// or just another logger thread which
// sleeps for a bit between each flush
// Or this could be gatted by waiting on CVs
//  https://stackoverflow.com/questions/2850091/wait-on-multiple-condition-variables-on-linux-without-unnecessary-sleeps/2873123
void flush_queues(logger_struct *ls)
{
}


