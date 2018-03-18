/**
 * @brief Logger opeartions
 * 
 * @file logger.c
 * @author Aakash Kumar 
 * @author Miles Frain 
 * @date 2018-03-17
 */
#include "logger.h"

// TLS members
__thread sequeue *logging_queue_tls;
__thread int queue_index_tls;



void *log_flush_task(void *arg)
{

    logger_struct *ls = (logger_struct *)arg;
    ls->keep_logger_alive=1;

    while (ls->keep_logger_alive)
    {
        // Scanning all queues, but could also just stop at
        // first null queue, since we aren't deleting any queues.
        for (int i = 0; i < MAX_QUEUES; i++)
        {
            sequeue *q = ls->queues[i];
            if (q)
            {
                char *msg;
                // While more data available - no blocking
                while ((msg = (char *)sequeue_read_next(q, true)))
                {
                    printf("From Logger queue %d: %s", i, msg);
                    fprintf(ls->fp, "From Logger queue %d: %s", i, msg);
                    sequeue_done_reading(q);
                    fflush(ls->fp);
                }
            }
        }
        usleep(1000); // 1 ms sleep
    }
    // Will exit when keep alive becomes zero
    fclose(ls->fp);
    free(logging_queue_tls);
    free(ls->queues_lock_m);
    printf("Logger Closed\n");
    return NULL;
}

void initialize_logger(logger_struct *ls, char *filename)
{
    // Zero-out queues array
    memset(ls->queues, 0, sizeof(ls->queues));

    // Open file
    if (!(ls->fp = fopen(filename, "w")))
    {
        printf("Error opening file %s\n", filename);
        abort();
    }

    if (!(ls->queues_lock_m = malloc(sizeof(pthread_mutex_t))))
    {
        abort();
    }
    if (pthread_mutex_init(ls->queues_lock_m, NULL))
    {
        abort();
    }

    pthread_t tid;
    ls->threadid=pthread_create(&tid, NULL, log_flush_task, ls);
    //pthread_join(tid, NULL);
}

void destroy_logger(logger_struct *ls)
{
    ls->keep_logger_alive=0; // This will kill the logger task
}

void enable_logging_in_thread(logger_struct *ls)
{
    logging_queue_tls = NULL;
    pthread_mutex_lock(ls->queues_lock_m);
    for (int i = 0; i < MAX_QUEUES; i++)
    {
        if (!ls->queues[i])
        {
            if (!(logging_queue_tls = malloc(sizeof(sequeue))))
            {
                printf("error allocating TLS queue\n");
            }
            sequeue_init(logging_queue_tls, MAX_LOG_LINE, MAX_PENDING_LOGS);
            ls->queues[i] = logging_queue_tls;
            break;
        }
    }
    pthread_mutex_unlock(ls->queues_lock_m);

    if (!logging_queue_tls)
    {
        // Could not find available logging queue
        abort();
    }
}

void log_printf(char *fmt, ...)
{
    char *spot = (char *)sequeue_next_empty(logging_queue_tls, false);
    va_list args;
    va_start(args, fmt);
    vsnprintf(spot, MAX_LOG_LINE, fmt, args);
    sequeue_done_writing(logging_queue_tls);
    va_end(args);
}