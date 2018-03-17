# pragma once

#ifdef BBB
#include "sensordata.h"
#else
#include "dummydata.h"
#endif

typedef struct thread_param
{
    bdqueue * temp_q;
    bdqueue * light_q;

    char * logfile_name;

    // Can use sig_atomic_t if this was being
    // modified in any more complicated manner
    volatile int keep_thread_alive;

} thread_param_t;

typedef enum
{
    HEARTBEAT,
    TEMPERATURE,
    LIGHT,
} data_header_type_t;

extern char* data_header_type_strings[] ;

typedef struct
{
    data_header_type_t type;

    // TODO: Change to timerspec struct for precise time
    long req_time;
    long res_time;

    union
    {
        struct
        {
            float value;
        } temperature;

        struct
        {
            float value;
        } light;
    };
} logged_data_t;

/**
 * @brief Initializes the light sensor
 * this will also do the startup test to see if I2C comms
 * is working
 * and sets up timer to periodically log temperature
 *
 *
 */
void * temperature_task(void * thread_param);

/**
 * @brief Initializes the light sensor
 * Does the startup test, and checks the sensor id
 * Sets up periodic timer to log light data
 *
 */
void * light_task(void * thread_param);

int thread_param_init(thread_param_t *param);

logged_data_t * add_to_bdqueue(bdqueue * queue,data_header_type_t type);

int queue_init(bdqueue ** queue);

void printQ(logged_data_t * msg);