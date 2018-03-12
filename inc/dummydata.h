#ifndef DUMMY_DATA_H
#define DUMMY_DATA_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TEMP_MAX 35
#define TEMP_MIN 15

#define LIGHT_MAX 80
#define LIGHT_MIN 50

time_t t;
/**
 * @brief Get the temp object
 * 
 * @return float 
 */
float get_temp();

/**
 * @brief Get the light object
 * 
 * @return float 
 */
float get_light();

#endif