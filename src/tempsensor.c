#include "tempsensor.h"

int main()
{

	
    int file;
    char filename[40];
    int addr = 0x48;        // The I2C address of temp sensor

    sprintf(filename,"/dev/i2c-2");
    if ((file = open(filename,O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }

    if (ioctl(file,I2C_SLAVE,addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }
	
    	uint16_t temp;
	float temperature;
	uint8_t buf[2] = {0,0};
	while(1)
{
	read(file,buf,2);

	for(int i=0;i<2;i++)
		printf("Byte %d is %x ",i,buf[i]);
	printf("\n");
	temp=(uint16_t)(buf[0]<<4);
	temp|=buf[1]>>4;
	printf("Final val is %x\n",temp);
	temperature=temp*0.0625;
	printf("Temperature is %lf degC , %lf\n"
			,temperature,TODEGF(temperature));
}
	
}


