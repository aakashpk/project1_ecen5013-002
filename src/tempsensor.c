#include "tempsensor.h"

/**
 * @brief Open a file handler to I2C bus and setup IOCTL
 *
 * @return int8_t file handler if successful, -1 if failure
 */
int8_t i2c_open_temp()
{
    int8_t file;
    file = i2c_open();

    if (file < 0)
        return -1;

    if (ioctl(file, I2C_SLAVE, TEMP_SENSOR_ADDR) < 0)
    {
        perror("IOCTL failed for tempsensor");
        return -1;
    }

    return file;
}

/**
 * @brief Read a register from temperature sensor
 *
 * @param reg register to read from
 * @return uint16_t value read from register in
 * proper 16 bit order
 */
uint16_t read_reg_temp(uint8_t reg)
{
    int8_t file;
    uint16_t buffer;

    file = i2c_open_temp();

    if (file < 0)
        return 1;

    if (write(file, &reg, 1) < 0) // Write to bus, which register has to be read
        return 1;

    if (read(file, &buffer, 2) < 0)
    {
        perror("Error reading from register");
        i2c_close(file);
        return 1;
    }

    i2c_close(file);

    return BYTESWAP(buffer);
}

/**
 * @brief Write a value to a register in the
 * temperature sensor
 *
 * @param reg The register in the sensor to write to
 * @param value 16 bit value to write to the register
 * @return int8_t return 0 if write succeds, else 1
 *
 */
int8_t write_reg_temp(uint8_t reg, uint16_t value)
{
    int8_t file;
    uint32_t buffer;

    // buffer=((((uint32_t)(TEMP12TOU16(value)))<<8)|((uint32_t)reg));
    buffer = ((((uint32_t)(BYTESWAP(value))) << 8) | ((uint32_t)reg));

    file = i2c_open_temp();

    if (file < 0)
        return -1;

    if (write(file, &buffer, 3) < 1)
    {
        perror("Error writing to register");
        i2c_close(file);
        return -1;
    }

    i2c_close(file);

    return 0;
}

/**
 * @brief Temperature sensor
 *
 * @return int8_t
 */
int8_t temp_sensor_init(void)
{
    uint16_t testval = 0x4b30;

    if (read_reg_temp(TEMP_VAL) == 1)
        return -1;

    if (write_reg_temp(TLOW, testval) == -1)
        return -1;

    if (read_reg_temp(TLOW) != testval)
        return -1;

    if (write_reg_temp(TLOW, 0x4b00) == -1) // Put it back to default value
        return -1;

    // Return 0 when all init works fine
    return 0;
}

void temp_sensor_raise_alert()
{
    // read temperature
    // Change high val to less than the read value and raise alert

    uint16_t value;
    value = read_reg_temp(TEMP_VAL);

    write_reg_temp(THIGH, (value - (uint16_t)256));

    read_reg_temp(TEMP_VAL);
    read_reg_temp(TEMP_VAL);
    read_reg_temp(TEMP_VAL);
}