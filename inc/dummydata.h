#ifndef DUMMY_DATA_H
#define DUMMY_DATA_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define TEMP_MAX 35
#define TEMP_MIN 15

#define LIGHT_MAX 80
#define LIGHT_MIN 50

time_t t;

float get_temp();

float get_light();

#endif