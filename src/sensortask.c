#include "sensortask.h"

float get_temp(uint8_t unit)
{
    float temperature;

    // read temperature register shift to remove the lagging 0 
    //and convert to degC value
    temperature=TOTEMPVAL((read_reg_temp(TEMP_VAL)>>4));
    
    //(((uint16_t)~d)&(uint16_t)0x0FFF)+1
    
    // Convert degC to the requested unit
    //1 for degF, 2 for K, default - degC
    if(unit==0) return temperature;
    else if(unit==1) return TODEGF(temperature);
    else if(unit==2) return TODEGK(temperature);
    else return temperature;
}

float get_light()
{
    float light;

    // read channel 1 for visible light and multiply by channel1 rv gain 
    light=read_reg_light_word(DATA1) * RV_FLU_LOWGAIN_CH1;
    
    return light;

} 

void sensors_test_print(void)
{
    printf("Light %lf lux ",get_light());
    printf("Temperature:%lf degC, ",get_temp(0) );
    
    #ifdef SENSORDEBUG
    printf("Data0:0x%x ,",read_reg_light_word(DATA0));
    printf("Data1:0x%x ,",read_reg_light_word(DATA1));
    
    printf("Config reg:0x%x, ",(read_reg_temp(CONFIG)));               
    printf("TLOW:%x, THIGH:%x\n",(read_reg_temp(TLOW)),read_reg_temp(THIGH));
    
    #endif
    printf("\n"); 
}

/**
 * @brief Initializes the light sensor
 * this will also do the startup test to see if I2C comms
 * is working
 * and sets up timer to periodically log temperature
 * 
 * 
 */
void * temperature_task()
{

    return NULL;
}

/**
 * @brief Initializes the light sensor
 * Does the startup test, and checks the sensor id
 * Sets up periodic timer to log light data 
 * 
 */
void * light_task()
{

    return NULL;

}
