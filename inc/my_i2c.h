#ifndef MY_I2C_H
#define MY_I2C_H

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



#define I2CBUSNAME "/dev/i2c-2" 



uint8_t i2c_open();


void i2c_close(uint8_t file);

#endif
