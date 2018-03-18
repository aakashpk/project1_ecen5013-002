/**
 * @brief Light sensor I2C operations
 * 
 * @file lightsensor.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-17
 */
#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include "my_i2c.h"

#include <string.h>

#define LIGHT_SENSOR_ADDR (uint8_t)0x39

#define I2CBUSNAME "/dev/i2c-2"

// Register Addresses For byte reads
#define CONTROL (uint8_t)0x80
#define TIMING (uint8_t)0x81
#define THRESHLOWLOW (uint8_t)0x82
#define THRESHLOWHIGH (uint8_t)0x83
#define THRESHHIGHLOW (uint8_t)0x84
#define THRESHHIGHHIGH (uint8_t)0x85
#define INTERRUPT (uint8_t)0x86
#define ID (uint8_t)0x8A
#define DATA0LOW (uint8_t)0x8C
#define DATA0HIGH (uint8_t)0x8D
#define DATA1LOW (uint8_t)0x8E
#define DATA1HIGH (uint8_t)0x8F

//Register Addresses for word reads
#define THRESHLOW (uint16_t)0xA2  // Low threshold 16 bit limit
#define THRESHHIGH (uint16_t)0xA4 // High threshold 16 bit limit
#define DATA0 (uint16_t)0xAC      // Channel 0 16 bit data
#define DATA1 (uint16_t)0xAE      // Channel 1 16 bit data

//Register values for sensor operations
#define TURN_ON (uint8_t)0x03
#define HIGH_GAIN (uint8_t)0x10
#define LIGHT_INT_TIME(x) (uint8_t)((((uint8_t)(x))) & (uint8_t)(0x03))

// Gain values
#define RV_FLU_LOWGAIN_CH0 2.3
#define RV_FLU_LOWGAIN_CH1 0.25

#define RV_INC_LOWGAIN_CH0 9
#define RV_INC_LOWGAIN_CH1 4.5

//
#define LIGHT_INTR(x) (uint8_t)((((uint8_t)(x) << 4)) & (uint8_t)(0x30))

/**
 * @brief Initialize light sensor by
 * writing to the control register 
 * startup test by reading the ID register
 * 
 * 
 * @return int8_t 
 */
int8_t light_sensor_init();

/**
 * @brief Open the I2C bus for comms with 
 * the light sensor
 * 
 * @return int8_t return -1 if error
 *  0 if successful
 */
int8_t i2c_open_light();

/**
 * @brief Read a single byte of data 
 * from the light sensor
 *
 * @param reg the register to read
 * @return uint8_t byte value from the light sensor register
 */
uint8_t read_reg_light_byte(uint8_t reg);

/**
 * @brief Read a word of data from the light sensor
 * 
 * @param reg register to read with read word bit set
 * @return uint16_t value of the 16 bit register from 
 * the light sensor
 */
uint16_t read_reg_light_word(uint8_t reg);

/**
 * @brief Write a byte of data to 
 * a register on the light sensor
 * 
 * @param reg register to write to
 * @param value value to be written
 * @return int8_t return 0 if successful
 * -1 if failure
 */
int8_t write_reg_light_byte(uint8_t reg, uint8_t value);

/**
 * @brief Write a word of data to 
 * a register on the light sensor
 * 
 * @param reg register to write to
 * @param value value to be written
 * @return int8_t return 0 if successful
 * -1 if failure
 */
int8_t write_reg_light_word(uint8_t reg, uint16_t value);

#endif
