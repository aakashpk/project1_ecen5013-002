/**
 * @brief Sensor data operations
 * 
 * @file sensordata.c
 * @author Aakash Kumar @author Miles Frain 
 * @date 2018-03-17
 */
#include "sensordata.h"
#include <math.h>

float get_temp(temp_unit_t unit)
{
    float temperature;
    uint16_t reg;

    // read temperature register shift to remove the lagging 0
    //and convert to degC value
    reg = read_reg_temp(TEMP_VAL);

    temperature = TOTEMPVAL((reg >> 4));

    //(((uint16_t)~d)&(uint16_t)0x0FFF)+1

    // Convert degC to the requested unit
    //1 for degF, 2 for K, default - degC
    if (unit == 0)
        return temperature;
    else if (unit == 1)
        return TODEGF(temperature);
    else if (unit == 2)
        return TODEGK(temperature);
    else
        return temperature;
}

float get_light()
{
    float luxVal = 0.0, ch0, ch1, ratio;

    // read channel 1 for visible light and multiply by channel1 rv gain
    ch0 = read_reg_light_word(DATA0); // * RV_FLU_LOWGAIN_CH1;
    ch1 = read_reg_light_word(DATA1);

    if (ch0 == 0)
        ratio = ch1;
    else
        ratio = ch1 / ch0;

    if (ratio <= 0.5)
    {
        luxVal = (0.0304 * ch0) - ((0.062 * ch0) * (pow((ch1 / ch0), 1.4)));
    }
    else if (ratio <= 0.61)
    {
        luxVal = (0.0224 * ch0) - (0.031 * ch1);
    }
    else if (ratio <= 0.8)
    {
        luxVal = (0.0128 * ch0) - (0.0153 * ch1);
    }
    else if (ratio <= 1.3)
    {
        luxVal = (0.00146 * ch0) - (0.00112 * ch1);
    }

#ifdef SENSORDEBUG
    printf("Ch0:%lf Ch1:%lf Ratio: %lf , luxValue: %lf\n", ch0, ch1, ratio, luxVal);
#endif

    return luxVal;
}

void sensors_test_print(void)
{
    printf("Light %lf lux ", get_light());
    printf("Temperature:%lf degC, ", get_temp(0));

#ifdef SENSORDEBUG
    printf("Data0:0x%x ,", read_reg_light_word(DATA0));
    printf("Data1:0x%x ,", read_reg_light_word(DATA1));

    printf("Config reg:0x%x, ", (read_reg_temp(CONFIG)));
    printf("TLOW:%x, THIGH:%x\n", (read_reg_temp(TLOW)), read_reg_temp(THIGH));

#endif
    printf("\n");
}
