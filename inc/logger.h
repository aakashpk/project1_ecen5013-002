/**
 * @brief Logging library
 *
 * @file logger.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-17
 */

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>

#include "single_ended_queue.h"

/*
 * Initialization of primary logging thread in main Thread-local storage of
 * logging queue in each thread. Each logging queue is registered with main
 * logger (either through global logging pointer, or logger pointer passed into
 * each thread). Logging posts by thread should not be blocking. Consuming log
 * messages from thread's queue handled by logger thread.
 * Was considering using single condition variable for notifying logger upon
 * adding records. Will cause minimal blocking if multiple threads attempt to
 * signal simultaniously. Although simple polling seems better as described in
 * log_flush_task() notes.
 */

// Maximum number of logging queues / tasks which can post logs
#define MAX_QUEUES 256

typedef struct
{
    // Todo - may want to use a list instead of array in case we want to remove queues,
    // although for our use, we aren't planning on removing any queues.
    sequeue *queues[MAX_QUEUES];
    pthread_mutex_t *queues_lock_m;
    FILE *fp;
    // No log rotation for this version of project

    pid_t threadid ;
    volatile sig_atomic_t keep_logger_alive; // To shutdown the thread
} logger_struct;

// Could eventually convert to singleton, but that's low priority now.
//logger_struct logger_singleton;

// Maximum length in characters of each log line
#define MAX_LOG_LINE 256
// Maximum number of pending logs (queue depth). Rounds up to 64
#define MAX_PENDING_LOGS 50

/*
Main logger thread constantly checks each queue
    Can stop checking at first null queue,
     but could also modify to check all queues
        (slightly slower, but allows for deleting queues)
        Better option is to use linked lists instead of array to track queues.
    Checks with force_nonblocking.
    Sleeps for 1 ms before each recheck cycle to keep CPU use low
        Sleep duration is tradeoff of CPU use vs delivery to file lag (lag likely not major problem).
        Choice of using sleep instead of signaling and blocking is
         another tradeoff of CPU use and delivery lag vs threads being
         blocked while writing to non-full queues as described below.
    Alternate choice besides sleep is to use CV blocking within log processing.
        Requires using read-only total_writes counter, which is compared to previous_total_writes.
        If counters different, saves new total (no mutex protection required, since context swap will only cause
        potentially eroneous difference, which will trigger another cycle of processing empty queues).
        If counters same, waits until different and signaled.
        The total_writes counter is incremented under mutex protection upon queue write from any logging task, and
         CV signal also generated.
        Biggest problem with this scheme is that tasks are all blocked by each other when incrementing and signaling,
         which kinda defeats the purpose of having dedicated queues for each task.
    Delivery latency likey better than blocking, so going with sleep approach to start with.
        Also less complex.
*/
/**
 * @brief Logging task as independ thread, which constantly checks queues for new logs to write to file.
 *
 * @param arg pointer to logger struct
 * @return void* NA
 */
void *log_flush_task(void *arg);

/**
 * @brief Logger initialization function to be called by program entry (main)
 *
 * @param ls pointer to logger struct
 * @param filename name of files to write logs to
 */
void initialize_logger(logger_struct *ls, char *filename);

/**
 * @brief Logger initialization function to be called by each thread which wishes to log.
 *
 * @param ls pointer to logger struct. This needs to be the same logger struct that was previously initialized by main.
 */
void enable_logging_in_thread(logger_struct *ls);

/**
 * @brief Printf-style function for posting logs to file.
 * This can be called by any thread which previously called initialize_logger().
 *
 * @param fmt format string
 * @param ... optional arguments
 */
void log_printf(char *fmt, ...);

void destroy_logger(logger_struct *ls);