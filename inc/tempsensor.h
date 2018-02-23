#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>


#define TEMP_SENSOR_ADDR 0x48

#define TODEGK(c) ((c)+273)

#define TODEGF(c) (((c)*9/5)+32)


/**
 * @brief Get temperature reading from the TMP102 sensor
 *
 * @return 16 bit temperature value
 */
int get_temp();


/**
 * @brief Function to read a register from the TMP102 sensor
 *
 * @param reg register to be read
 *
 * @return the value read from the register
 */
uint8_t read_reg(uint8_t * reg);

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
uint8_t write_reg(uint8_t * reg);






#endif
