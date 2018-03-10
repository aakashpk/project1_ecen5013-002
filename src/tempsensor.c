#include "tempsensor.h"

int8_t i2c_open_temp()
{
    int8_t file;
    file=i2c_open();

    if (ioctl(file,I2C_SLAVE,TEMP_SENSOR_ADDR) < 0) {
        perror("IOCTL failed for tempsensor");
        return -1;
    }

    return file;
}


int16_t read_reg_temp(uint8_t reg)
{
    int8_t file;
    int16_t buffer;

    file=i2c_open_temp();
    
    write(file,&reg,1); // Write to bus, which register has to be read

    if(read(file,&buffer,2)<0)
    {
        printf("Error reading from register");
        i2c_close(file);
        return -1;
    }

    i2c_close(file);
    return buffer;
}

int8_t write_reg_temp(uint8_t reg,uint16_t value)
{
    int8_t file;

    file=i2c_open_temp();

    write(file,&reg,1);

    if(write(file,&value,3)!=1)
    {
        printf("Error writing to register\n");
        i2c_close(file);
        return -1;
    }

    i2c_close(file);

    return 0;

    //TODO: Add a return for this ??
}

int8_t temp_sensor_init(void)
{

    // Should return 0 when all init works fine
    return 0;
}