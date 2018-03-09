

#ifdef BBB
#include "tempsensor.h"
#include "lightsensor.h"
#else
#include "dummydata.h"
#endif
int main()
{
    #ifndef BBB
    // Code that will execute only on host
    //while(1)
    {
        printf("Temperature is %lf , light is %lf\n",get_temp(),get_light());
    }
    
    #endif

    #ifdef BBB
    // Code that will execute only on BBB
    printf("TLOW:%x, THIGH:%x\n",(read_reg_temp(TLOW)),read_reg_temp(THIGH));

    int file;
    uint8_t a=0x03;
    uint16_t b=0xabcd;
//    uint8_t c=0xef;
  //  uint8_t d=0xab;
    file=open(I2CBUSNAME,O_WRONLY);
    ioctl(file,I2C_SLAVE,TEMP_SENSOR_ADDR);
    printf("%d\n",write(file,&a,2));
    //printf("%d : val: %x\n",write(file,&b,3),b);
    printf("%d\n",write(file,&b,3));
//    printf("%d\n",write(file,&d,3));
    close(file);

    
//    while(1)
    {
     //write_reg_temp(TEMP_VAL,0x00);
      
    printf("Config reg:0x%x, ",(read_reg_temp(CONFIG)));
    printf("Temp:%lf degC, ",
                get_temp(0) );
    printf("TLOW:%x, THIGH:%x\n",(read_reg_temp(TLOW)),read_reg_temp(THIGH));

    }

    #endif
}
