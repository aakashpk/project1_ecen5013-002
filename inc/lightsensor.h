#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

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


#define LIGHT_SENSOR_ADDR 0x39

#define I2CBUSNAME "/dev/i2c-2"

uint16_t get_light_value();


void light_sensor_init();

uint8_t i2c_open_light();

uint8_t read_reg_light();

uint8_t write_reg_light();


#endif
