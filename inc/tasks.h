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
    bdqueue * system_q;
    volatile int keep_thread_alive;

} thread_param_t;

typedef struct logged_data
{
    uint8_t type;
    long timestamp;
    float value;
}logged_data_t;

void * temperature_task(void * thread_param);

void * light_task(void * thread_param);

int thread_param_init(thread_param_t *param);

int queue_init(bdqueue ** queue);