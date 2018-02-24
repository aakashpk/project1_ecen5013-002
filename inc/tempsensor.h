#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include "my_i2c.h"

#define TEMP_SENSOR_ADDR 0x48

#define TODEGK(c) ((c)+273)

#define TODEGF(c) (((c)*9/5)+32)


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
uint8_t write_reg_temp(uint8_t reg,uint8_t value);






#endif
