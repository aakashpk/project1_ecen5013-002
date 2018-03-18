/**
 * @brief Thread creation functions for the 
 * Sensor tasks 
 * 
 * @file tasks.h
 * @author Aakash Kumar 
 * @author Miles Frain 
 * @date 2018-03-17
 */
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

/**
 * @brief initializes parameter variable to be passed 
 * into the thread creation functions
 * 
 * @param param 
 * @return int 
 */
int thread_param_init(thread_param_t *param);


logged_data_t *dispatch_request(bdqueue *queue, data_header_type_t type);

/**
 * @brief Initializes a queue from the bdqueue 
 * give the pointer to the head
 * 
 * @param queue pointer to the queue within the
 * parameter structure
 * 
 * @return int 0 if success, -1 for failure
 */
int queue_init(bdqueue **queue);

// Unused in final usage, only used for testing
void printQ(logged_data_t *msg);


/**
 * @brief  Clears the keep alive variable 
 * killing the sensor taks threads
 * 
 * @param thread_param 
 */
void kill_tasks(void *thread_param);
