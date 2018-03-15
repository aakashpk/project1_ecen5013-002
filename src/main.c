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
    static char logfilename[]="project1log.log";
    extern char *optarg;
	extern int optind;
    int optret;

    optret=getopt(argc, argv, "f:");

    if(argc < 3 || optret!='f')
    {
        printf("Usage is project1 -f logfilename\n \
        File option not proper, using project1log.log\n");
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

    /* TODO: Make an array of all thread functions and move the thread creating inside a loop
     Parameters to be passed to the thread would be logger referrences,
     those can also be put into an array */

    if(pthread_create(&threadIDs[0],NULL,socket_thread,NULL)!=0)
		printf("Thread 1 creation failed\n");

    printf("Thread created with ID %ld\n",threadIDs[0]);

    printf("Temp is %lf, light is %lf\n",get_temp(),get_light());
    
    pthread_join(threadIDs[0],NULL);

    return 0;
    #endif
    
    #ifdef BBB
    // Code that will execute only on BBB

    if(temp_sensor_init()==0) LOG_STR("Temp Sensor Self Test Done\n");
    else 
    {
        printf("Temp Sensor Init Failed\n");
        exit(1);
    }

    if(light_sensor_init()==0) LOG_STR("Light Sensor Self Test Done\n");
    else 
    {
        printf("Light Sensor Init Failed\n");
        exit(1);
    }
      

    temp_sensor_raise_alert();

    
   while(1)
    {
     
	    //sensors_test_print();	

    } 
    
    #endif

}
