/**
 * @brief I2C file operation wrappers
 * 
 * @file my_i2c.h
 * @author Aakash 
 * @author Miles Frain
 * @date 2018-03-17
 */
#ifndef MY_I2C_H
#define MY_I2C_H

#define _XOPEN_SOURCE 500

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
//#include <i2c/smbus.h>

#define I2CBUSNAME "/dev/i2c-2"

/**
 * @brief Open I2C bus
 * 
 * @return int8_t file descriptor
 */
int8_t i2c_open();

/**
 * @brief Close I2C bus
 * 
 * @param file I2C bus to close
 */
void i2c_close(int8_t file);

#endif
