/**
 * @brief Read and write from the temperature sensor
 * registers
 * 
 * @file tempsensor.h
 * @author Aakash Kumar @author Miles Frain 
 * @date 2018-03-17
 */
#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include "my_i2c.h"

#define LOG_STR printf // temporary till actual logging is setup

// Temp sensor address
#define TEMP_SENSOR_ADDR (uint8_t)0x48

// Sensor register addresses
#define TEMP_VAL (uint8_t)0x00
#define CONFIG (uint8_t)0x01
#define TLOW (uint8_t)0x02
#define THIGH (uint8_t)0x03

// Temperature unit conversion
#define TODEGK(c) ((c) + 273)
#define TODEGF(c) (((c)*9 / 5) + 32)

//*** Byte shift required for register read and write operations

//Convert 16bit register read to 12 bit temp with the lowest byte shifted out
#define U16TOTEMPOUT(reg_16) (((((reg_16) >> 8) & 0x00F0) | (((reg_16) << 8) & 0xFF00)) >> 4)

// Converts 12 bit register value to temperature,
//takes care of -ve temperature as well
#define TOTEMPVAL(reg12read) ((reg12read) > 0x7FF) ? ((reg12read * 0.0625) - 256) : (reg12read * 0.0625)

// Register read returns high byte as low byte after read, swap bytes to get correct value
#define BYTESWAP(reg_16) ((((reg_16) >> 8) & 0x00FF) | (((reg_16) << 8) & 0xFF00))

#define TEMP12TOU16(reg_12) ((((reg_12) >> 4) & 0x00FF) | (((reg_12) << 12) & 0xF000))

#define TEMP_SD (uint16_t)0x0100
#define TEMP_TM (uint16_t)0x0200
#define TEMP_POL (uint16_t)0x0400
#define TEMP_FAULT(x) (uint16_t)((((uint16_t)(x) << 3)) & (uint16_t)(0x1800))
#define TEMP_CONV_RATE(x) (uint16_t)((((uint16_t)(x) << 6)) & (uint16_t)(0x00C0))
#define TEMP_EM_MODE (uint16_t)0x0010

int8_t i2c_open_temp();

/**
 * @brief Function to read a register from the TMP102 sensor
 *
 * @param reg register to be read
 *
 * @return the value read from the register
 */
uint16_t read_reg_temp(uint8_t reg);

/**
 * @brief Initialize temperature sensor
 */
int8_t temp_sensor_init(void);

/**
 * @brief Write a register in the temperature sensor
 *
 * @param reg register to be written
 * @param value 0 for success, -1 for failure
 */
int8_t write_reg_temp(uint8_t reg, uint16_t value);

/**
 * @brief raise alert on the sensor by
 * reducing the THIGH value to below the currently read value
 * 
 */
void temp_sensor_raise_alert();

#endif
