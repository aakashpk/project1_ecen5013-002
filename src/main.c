//#define BBB

#include <getopt.h>
#ifdef BBB
#include "tempsensor.h"
#include "lightsensor.h"
#include "sensortask.h"
#else
#include "dummydata.h"
#include "socketserver.h"
#include <pthread.h>
#endif

int main(int argc, char **argv)
{
    static char logfilename[]="project1log.txt";
    extern char *optarg;
	extern int optind;
    int optret;

    optret=getopt(argc, argv, "f:");

    if(argc < 3 || optret!='f')
    {
        printf("Usage is project1 -f logfilename\n \
        File option not proper, using project1log.txt\n");
    }
    else
        strcpy(logfilename,optarg);

    printf("Log Filename is %s\n",logfilename);

    #ifndef BBB

    // Code that will execute only on host
    
    pthread_t threadIDs[1];
    /*
    Create logger thread
    create system logging thread
    create temp sensor thread
    create light sensor thread
    Create external request socket server
    */

    if(pthread_create(&threadIDs[0],NULL,socket_thread,NULL)!=0)
		printf("Thread 1 creation failed\n");

    printf("Thread created with ID %ld\n",threadIDs[0]);

    printf("Temp is %lf, light is %lf",get_temp(),get_light());
    
    pthread_join(threadIDs[0],NULL);

    return 0;
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
     
    //for(uint8_t i=128;i<144;i++)
        //printf("Reg 0x%x:0x%x ,",i,read_reg_light_byte(i));

    
    } 
    
    printf("%x %x",read_reg_light_word(THRESHLOW),read_reg_light_word(THRESHHIGH));

    #endif
}
