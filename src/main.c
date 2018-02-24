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
    }

}
