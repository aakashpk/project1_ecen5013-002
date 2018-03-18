#pragma once

#ifdef BBB
#include "sensordata.h"
#else
#include "dummydata.h"
#endif

#include "logger.h"
#include "socketserver.h"
#include "bidirectional_queue.h"
#include "message_data_type.h"

#define THREAD_NUMBER 3

typedef struct thread_param
{
    bdqueue *temp_q;
    bdqueue *light_q;

    char *logfile_name;

    // Atomic volatile to kill thread from main
    volatile sig_atomic_t keep_thread_alive;
    logger_struct * logger;
} thread_param_t;


/**
 * @brief Initializes the light sensor
 * this will also do the startup test to see if I2C comms
 * is working
 * and sets up timer to periodically log temperature
 *
 *
 */
void *temperature_task(void *thread_param);

/**
 * @brief Initializes the light sensor
 * Does the startup test, and checks the sensor id
 * Sets up periodic timer to log light data
 *
 */
void *light_task(void *thread_param);

int thread_param_init(thread_param_t *param);

logged_data_t *add_to_bdqueue(bdqueue *queue, data_header_type_t type);

int queue_init(bdqueue **queue);

void printQ(logged_data_t *msg);

void kill_tasks(void *thread_param);
