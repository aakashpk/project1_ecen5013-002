#include "dummydata.h"

float get_temp()
{
    srand( time (&t));
    return ((rand() % (TEMP_MAX + 1 - TEMP_MIN)) + TEMP_MIN);
}

float get_light()
{
    srand( time (&t));
    return ((rand() % (LIGHT_MAX + 1 - LIGHT_MIN)) + LIGHT_MIN);
}