#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include "my_i2c.h"

#define TEMP_SENSOR_ADDR 0x48

#define TODEGK(c) ((c)+273)

#define TODEGF(c) (((c)*9/5)+32)

#define CONFIG 1
#define TLOW 2
#define THIGH 3

#define U16TOTEMPOUT(reg_16) (((((reg_16)>>4)&0x00F0)|(((reg_16)<<8)&0xFF00))>>4)

uint8_t i2c_open_temp();

/**
 * @brief Get temperature reading from the TMP102 sensor
 *
 * @return 16 bit temperature value
 */
float get_temp(uint8_t unit);


/**
 * @brief Function to read a register from the TMP102 sensor
 *
 * @param reg register to be read
 *
 * @return the value read from the register
 */
uint8_t read_reg_temp(uint8_t offset);

/**
 * @brief Initialize temperature sensor
 */
void temp_sensor_init();


/**
 * @brief
 *
 * @param reg
 *
 * @return
 */
void write_reg_temp(uint8_t reg,uint8_t value);






#endif
