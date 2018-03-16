//#define BBB

#include <getopt.h>
#ifdef BBB
#include "tempsensor.h"
#include "lightsensor.h"
#else
#include "dummydata.h"
#include "socketserver.h"
#include <pthread.h>
#endif

#define THREAD_NUMBER 3



int main(int argc, char **argv)
{
    static char logfilename[]="project1log.log";
    extern char *optarg;
	extern int optind;
    int optret;

    thread_param_t param1 ;
    param1.keep_thread_alive=1;

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
    
    pthread_t threadIDs[THREAD_NUMBER];
    
    // Array with functions for each of the tasks
    void * thread_functions[THREAD_NUMBER] ={socket_thread,temperature_task,light_task};

    // Array with parameters to be passed for each of these tasks
    // Maintain same order between the 2 arrays
    void * thread_parameters[THREAD_NUMBER] = {(void *)&param1,(void *)&param1,(void *)&param1};
    
    /*
    Create logger thread
    create system logging thread
    create temp sensor thread
    create light sensor thread
    Create external request socket server
    */


    for(int i=0;i<THREAD_NUMBER;i++)
    {
        if(pthread_create(&threadIDs[i],NULL,thread_functions[i],thread_parameters[i])!=0)
		    printf("Thread %d creation failed\n",i);

        else 
            printf("Thread created with ID %ld\n",threadIDs[0]);
    }

    
    for(int i=0;i<THREAD_NUMBER;i++)
    {
        pthread_join(threadIDs[i],NULL);
    }

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
