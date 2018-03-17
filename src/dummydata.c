#include "dummydata.h"

float get_temp(uint8_t unit)
{
    float temperature=0;
    srand( time (&t));
    temperature = (rand() % (TEMP_MAX + 1 - TEMP_MIN)) + TEMP_MIN;

    if(unit==0) return temperature;
    else if(unit==1) return TODEGF(temperature);
    else if(unit==2) return TODEGK(temperature);
    else return temperature;
}

float get_light()
{
    srand( time (&t));
    return ((rand() % (LIGHT_MAX + 1 - LIGHT_MIN)) + LIGHT_MIN);
}
