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
    uint8_t file;

    uint16_t temp;
    float temperature;

    uint8_t buf[2] = {0,0};

    file=i2c_open_temp(); // Open I2C bus with temp sensor addr

    read(file,buf,2); // read temp register

    i2c_close(file); // Close file

    // Convert read values to temp reading
    temp=(uint16_t)(buf[0]<<4);
    temp|=buf[1]>>4;

//    printf("b0:%x b1:%x temp:%x\n",buf[0],buf[1],temp);

    if(temp>0x7FF)
        temperature=((~temp)+1)*0.0625;
    temperature=temp*0.0625;

    // Convert degC to the requested unit
    //1 for degF, 2 for K, default - degC
    if(unit==0) return temperature;
    else if(unit==1) return TODEGF(temperature);
    else if(unit==2) return TODEGK(temperature);
    else return temperature;
}

uint8_t read_reg_temp(uint8_t reg)
{
    uint8_t file,buffer;
    file=i2c_open_temp();
    if(pread(file,&buffer,1,reg)<0)
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

void write_reg_temp(uint8_t reg,uint8_t value)
{
    uint8_t file,buffer;
    file=i2c_open_temp();

    if(pwrite(file,&value,1,reg)<0)
    {
        printf("Error reading from register\n");
        i2c_close(file);
        exit(1);
    }

    i2c_close(file);
    //TODO: Add a return for this ??
}

