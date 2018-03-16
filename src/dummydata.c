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


/**
 * @brief Initializes the light sensor
 * this will also do the startup test to see if I2C comms
 * is working
 * and sets up timer to periodically log temperature
 * 
 * 
 */
void * temperature_task(void * thread_param)
{
    thread_param_t * p1= (thread_param_t*)thread_param;
    while(p1->keep_thread_alive)
    {
        printf("Temperature is %lf \n",get_temp(0));
        sleep(1);
    }
    return NULL;
}

/**
 * @brief Initializes the light sensor
 * Does the startup test, and checks the sensor id
 * Sets up periodic timer to log light data 
 * 
 */
void * light_task(void * thread_param)
{
    thread_param_t * p1= (thread_param_t*)thread_param;
    while(p1->keep_thread_alive)
    {

        printf("Light is %lf \n",get_light());
        sleep(1);
    }
    return NULL;

}
