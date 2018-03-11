#include "lightsensor.h"


int8_t light_sensor_init()
{
    int ret;

    // Turn on sensor by writing 0x03 to the control register
    ret=write_reg_light_byte(CONTROL,TURN_ON);
    if(ret<0) return ret;

    uint8_t time_reg;
    time_reg=read_reg_light_byte(TIMING);
    time_reg|=HIGH_GAIN;
    
    // Set gain to high gain
    ret=write_reg_light_byte(TIMING,time_reg);
    if(ret<0) return ret;

    return 0;
    
}

int8_t i2c_open_light()
{
    int file;
    file = i2c_open();
    if (ioctl(file,I2C_SLAVE,LIGHT_SENSOR_ADDR) < 0) 
    {
        perror("Light sensor IOCTL failed"); // ERROR HANDLING;
        return -1;
    }

    return file;
}

uint8_t read_reg_light_byte(uint8_t reg)
{
    uint8_t file;
    uint8_t buffer;

    file=i2c_open_light();
    
    write(file,&reg,1); // Write to bus, which register has to be read

    if(read(file,&buffer,1)<0)
    {
        perror("Error reading from register");
        i2c_close(file);
        return -1;
    }

    i2c_close(file);
    return buffer;

}

uint16_t read_reg_light_word(uint8_t reg)
{
    uint8_t file;
    uint16_t buffer;

    file=i2c_open_light();
    
    write(file,&reg,1); // Write to bus, which register has to be read

    if(read(file,&buffer,2)<0)
    {
        perror("Error reading from register");
        i2c_close(file);
        return -1;
    }

    i2c_close(file);
    return buffer;

}



int8_t write_reg_light_byte(uint8_t reg,uint8_t value)
{
    uint8_t file;

    file=i2c_open_light();

    write(file,&reg,1);

    if(write(file,&value,1)<0)
    {
        perror("Error writing to file");
        i2c_close(file);
        return -1;
    }

    i2c_close(file);

    return 0;

}

int8_t write_reg_light_word(uint8_t reg,uint16_t value)
{
    uint8_t file;

    file=i2c_open_light();

    write(file,&reg,1);

    if(write(file,&value,4)<0)
    {
        perror("Error writing to file");
        i2c_close(file);
        return -1;
    }

    i2c_close(file);
    return 0;

}