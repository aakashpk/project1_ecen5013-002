/**
 * @brief Sensor Data read functions 
 * for temperature and light sensors
 * 
 * @file sensordata.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-17
 */

#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H


#include "tempsensor.h"
#include "lightsensor.h"

// Unit selection for temperature data
typedef enum 
{
    DEGC,
    DEGF,
    DEGK,
    
}temp_unit_t;

float get_temp(temp_unit_t unit);

float get_light();

void sensors_test_print(void);

#endif // End of header file