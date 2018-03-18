#ifndef DUMMY_DATA_H
#define DUMMY_DATA_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define TEMP_MAX 35
#define TEMP_MIN 15

#define LIGHT_MAX 80
#define LIGHT_MIN 50

// Temperature unit conversion
#define TODEGK(c) ((c) + 273)
#define TODEGF(c) (((c)*9 / 5) + 32)

time_t t; // For random data

/**
 * @brief Get the temp object
 *
 * @return float
 */
float get_temp(uint8_t unit);

/**
 * @brief Get the light object
 *
 * @return float
 */
float get_light();

#endif