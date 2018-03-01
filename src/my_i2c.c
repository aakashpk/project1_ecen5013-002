#include "my_i2c.h"



uint8_t i2c_open()
{
    uint8_t file;
    if ((file = open(I2CBUSNAME,O_RDWR)) < 0)
    {
        printf("Failed to open the bus.");
        // ERROR HANDLING;
        exit(1);
    }
    return file;
}


void i2c_close(uint8_t file)
{
    close(file);
}
