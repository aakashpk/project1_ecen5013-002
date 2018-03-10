#include "sensortask.h"

float get_temp(uint8_t unit)
{
    uint16_t temp,temp1;
    float temperature;

    temp=(uint16_t)read_reg_temp(TEMP_VAL); // read temperature register

    // Convert read values to temp reading
    // The 16 bit read value is in byte0 byte1 format change it
    // to byte 1 byte0 and shift 4 bits to remove last 4 unused bits
    temp1=U16TOTEMPOUT(temp);

    #ifdef DEBUG
    printf("temp:0x%x temp1:0x%x \n",temp,temp1);
    #endif

    //(((uint16_t)~d)&(uint16_t)0x0FFF)+1

    if(temp1>0x7FF)
        temperature=((temp1*0.0625)-256);
    else 
        temperature=temp1*0.0625;

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

/**
 * @brief Initializes the light sensor
 * this will also do the startup test to see if I2C comms
 * is working
 * and sets up timer to periodically log temperature
 * 
 * 
 */
void temperature_task()
{


}

/**
 * @brief Initializes the light sensor
 * Does the startup test, and checks the sensor id
 * Sets up periodic timer to log light data 
 * 
 */
void light_task()
{

}