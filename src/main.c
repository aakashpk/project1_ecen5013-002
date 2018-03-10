//#define BBB

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

    /*
    Create logger thread
    create system logging thread
    create temp sensor thread
    create light sensor thread
    */

    //pthread_create();
    
    #endif
    
    #ifdef BBB
    // Code that will execute only on BBB
/*    printf("TLOW:%x, THIGH:%x\n",(read_reg_temp(TLOW)),read_reg_temp(THIGH));

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
*/
    light_sensor_init();
    write_reg_light_word(THRESHLOW,(uint16_t)0xabcd);


  //  while(1)
    {
     //write_reg_temp(TEMP_VAL,0x00);
    
    //for(uint8_t i=128;i<144;i++)
        //printf("Reg 0x%x:0x%x ,",i,read_reg_light_byte(i));

    printf("Data0:0x%x ,",read_reg_light_word(DATA0));
    printf("Data1:0x%x ,",read_reg_light_word(DATA1));
    printf("Light %d lux ",get_light());
    //printf("Config reg:0x%x, ",(read_reg_temp(CONFIG)));
    printf("Temperature:%lf degC, ",get_temp(0) );
                
    //printf("TLOW:%x, THIGH:%x\n",(read_reg_temp(TLOW)),read_reg_temp(THIGH));
    
    printf("\n"); 
    } 
    
    printf("%x %x",read_reg_light_word(THRESHLOW),read_reg_light_word(THRESHHIGH));

    #endif
}
