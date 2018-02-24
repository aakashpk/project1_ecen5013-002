#include "lightsensor.h"


void light_sensor_init()
{

}

uint8_t i2c_open_light()
{
	uint8_t file;
	file = i2c_open();
	if (ioctl(file,I2C_SLAVE,LIGHT_SENSOR_ADDR) < 0) {
	       	printf("Failed to acquire bus access and/or talk to slave.\n");
        // ERROR HANDLING; 
		exit(1);
	}

	return file;


}

uint8_t read_reg_light()
{

}

uint8_t write_reg_light()
{

}



uint16_t get_light_value()
{

	
    int file;
    char filename[40];
    int addr = 0x39;        // The I2C address of light sensor
    
	sprintf(filename,"/dev/i2c-2");
	file = open(filename,O_RDWR);
	ioctl(file,I2C_SLAVE,addr); 
	
    	uint16_t temp;
	float temperature;
	uint8_t buf[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	while(1)
	{
		read(file,buf,16);

		for(int i=0;i<16;i++)
			printf("%x:%x, ",i,buf[i]);
		printf("\n");
	}
	
}
