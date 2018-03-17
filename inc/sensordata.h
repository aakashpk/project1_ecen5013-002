#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

/**
 * @brief 
 * 
 * @file sensordata.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-16
 */

#include "tempsensor.h"
#include "lightsensor.h"

#include "bidirectional_queue.h"

float get_temp(uint8_t unit);

float get_light();

void sensors_test_print(void);

#endif // End of header file