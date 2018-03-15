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
#define THRESHLOW (uint16_t)0xA2 // Low threshold 16 bit limit
#define THRESHHIGH (uint16_t)0xA4 // High threshold 16 bit limit
#define DATA0 (uint16_t)0xAC // Channel 0 16 bit data
#define DATA1 (uint16_t)0xAE // Channel 1 16 bit data


//Register values for sensor operations
#define TURN_ON (uint8_t)0x03
#define HIGH_GAIN (uint8_t)0x10
#define LIGHT_INT_TIME(x) (uint8_t)((((uint8_t)(x)))&(uint8_t)(0x03))

// Gain values
#define RV_FLU_LOWGAIN_CH0 2.3
#define RV_FLU_LOWGAIN_CH1 0.25

#define RV_INC_LOWGAIN_CH0 9
#define RV_INC_LOWGAIN_CH1 4.5

#define LIGHT_INTR(x) (uint8_t)((((uint8_t)(x)<<4))&(uint8_t)(0x30))

int8_t light_sensor_init();

int8_t i2c_open_light();

/**
 * @brief 
 * 
 * @param reg 
 * @return uint8_t 
 */
uint8_t read_reg_light_byte(uint8_t reg);

uint16_t read_reg_light_word(uint8_t reg);

int8_t write_reg_light_byte(uint8_t reg,uint8_t value);

int8_t write_reg_light_word(uint8_t reg,uint16_t value);


#endif
