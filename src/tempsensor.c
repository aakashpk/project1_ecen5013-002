#include "tempsensor.h"

uint8_t i2c_open_temp()
{
    uint8_t file;
    file=i2c_open();

    if (ioctl(file,I2C_SLAVE,TEMP_SENSOR_ADDR) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        // ERROR HANDLING;
        exit(1);
    }

    return file;
}


float get_temp(uint8_t unit)
{
    uint16_t temp,temp1;
    float temperature;

    temp=read_reg_temp(TEMP_VAL); // read temperature register

    // Convert read values to temp reading

    // The 16 bit read value is in byte0 byte1 format change it
    // to byte 1 byte0 and shift 4 bits to remove last 4 unused bits
    temp1=U16TOTEMPOUT(temp);

//    printf("temp:0x%x temp1:0x%x \n",temp,temp1);

    if(temp1>0x7FF)
        temperature=((~temp1)+1)*0.0625;
    temperature=temp1*0.0625;

    // Convert degC to the requested unit
    //1 for degF, 2 for K, default - degC
    if(unit==0) return temperature;
    else if(unit==1) return TODEGF(temperature);
    else if(unit==2) return TODEGK(temperature);
    else return temperature;
}

uint16_t read_reg_temp(uint8_t reg)
{
    uint8_t file;
    uint16_t buffer;

    file=i2c_open_temp();
    
    write(file,&reg,1); // Write to bus, which register has to be read

    if(read(file,&buffer,2)<0)
    {
        printf("Error reading from register");
        i2c_close(file);
        exit(1);
    }

    i2c_close(file);
    return buffer;
}

void temp_sensor_init()
{

}

void write_reg_temp(uint8_t reg,uint16_t value)
{
    uint8_t file;

    file=i2c_open_temp();

    write(file,&reg,1);

    if(write(file,&value,3)!=1)
    {
        printf("Error writing to register\n");
        i2c_close(file);
        exit(1);
    }

    i2c_close(file);

    //TODO: Add a return for this ??
}

