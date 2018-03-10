#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include "my_i2c.h"

#define TEMP_SENSOR_ADDR (uint8_t)0x48

#define TODEGK(c) ((c)+273)

#define TODEGF(c) (((c)*9/5)+32)

#define TEMP_VAL (uint8_t)0x00
#define CONFIG (uint8_t)0x01
#define TLOW (uint8_t)0x02
#define THIGH (uint8_t)0x03

#define U16TOTEMPOUT(reg_16) (((((reg_16)>>4)&0x00F0)|(((reg_16)<<8)&0xFF00))>>4)

#define BYTESWAP(reg_16) ((((reg_16)>>8)&0x00FF)|(((reg_16)<<8)&0xFF00))


int8_t i2c_open_temp();


/**
 * @brief Function to read a register from the TMP102 sensor
 *
 * @param reg register to be read
 *
 * @return the value read from the register
 */
int16_t read_reg_temp(uint8_t reg);

/**
 * @brief Initialize temperature sensor
 */
int8_t temp_sensor_init(void);


/**
 * @brief 
 * 
 * @param reg 
 * @param value 
 */
int8_t write_reg_temp(uint8_t reg,uint16_t value);






#endif
