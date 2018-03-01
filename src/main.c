#include "tempsensor.h"
#include "lightsensor.h"

int main()
{
    uint8_t file,t=0x01;
    float temperature;
    uint8_t buf[8]={1,1,1,1,1,1,1,1};
    while(1)
    {
        
        temperature=get_temp(0);
        printf("Temperature is %lf degC, %lf degF, %lf degK\n",
                temperature, TODEGF(temperature), TODEGK(temperature));
      //  printf("TLOW:0x %x, THIGH:0x%x\n",read_reg_temp(2),read_reg_temp(3));

         /*
    //file=i2c_open_temp();
    file=open(I2CBUSNAME,O_RDWR);
    ioctl(file,I2C_SLAVE,TEMP_SENSOR_ADDR) ;

    write(file,&t,1);

    read(file,buf,2);

    for(uint8_t i=0;i<8;i++)
    {
        //buf[i]=i2c_smbus_read_word_data(file, i);
        //buf[i]=i2c_smbus_read_byte_data ( file , i );
        printf("%d:%x ",i,buf[i]);
    }
    //i2c_smbus_write_byte_data ( file , 0x0a , val8 );
    printf("\n");
    i2c_close(file);
*/
    }

}
