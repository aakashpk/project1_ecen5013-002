/**
 * @brief I2C file opeartions
 * 
 * @file my_i2c.c
 * @author Aakash Kumar 
 * @author Miles Frain 
 * @date 2018-03-17
 */
#include "my_i2c.h"

int8_t i2c_open()
{
    int8_t file;
    if ((file = open(I2CBUSNAME, O_RDWR)) < 0)
    {
        perror("Bus Open Fail");
        return -1;
    }
    return file;
}

void i2c_close(int8_t file)
{
    close(file);
}
