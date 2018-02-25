#include "tempsensor.h"
#include "lightsensor.h"

int main()
{
    float temperature;

    while(1)
    {
        temperature=get_temp(0);
        printf("Temperature is %lf degC, %lf degF, %lf degK\n",
                temperature, TODEGF(temperature), TODEGK(temperature));
        printf("TLOW:0x %x, THIGH:0x%x\n",read_reg_temp(2),read_reg_temp(3));
    }

}
