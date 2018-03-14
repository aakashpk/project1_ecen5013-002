#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "tempsensor.h"
#include "lightsensor.h"

float get_temp(uint8_t unit);

float get_light();

void * temperature_task();

void * light_task();

#endif // End of header file