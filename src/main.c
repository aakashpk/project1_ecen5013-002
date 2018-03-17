//#define BBB

#include <getopt.h>
#include "socketserver.h"
#include <pthread.h>
#include "tasks.h"

#ifdef BBB
#include "tempsensor.h"
#include "lightsensor.h"
#include "sensordata.h"
#else
#include "dummydata.h"
#endif

#define THREAD_NUMBER 3

int main(int argc, char **argv)
{
    
    static char logfilename[]="project1log.log";
    extern char *optarg;
	extern int optind;
    int optret;

    
    // Get Log file name from command line
    // If not proper format use a default one
    optret=getopt(argc, argv, "f:");

    if(argc < 3 || optret!='f')
    {
        printf("Usage is project1 -f logfilename\n \
        File option not proper, using project1log.log\n");
    }
    else
        strcpy(logfilename,optarg);

    printf("Log Filename is %s\n",logfilename);

    // parameters to be passed during thread creation
    thread_param_t param1 ;

    
    if(thread_param_init(&param1)<0) 
        printf("Thread Param Init Failed\n");
    
    // Spawn the threads for the different tasks
    //

    pthread_t threadIDs[THREAD_NUMBER];
    
    // Array with functions for each of the tasks
    void * thread_functions[THREAD_NUMBER] ={socket_thread,temperature_task,light_task};

    // Array with parameters to be passed for each of these tasks
    // Maintain same order between the 2 arrays
    void * thread_parameters[THREAD_NUMBER] = {(void *)&param1,(void *)&param1,(void *)&param1};
    
    // TODO:
    //Create logger thread
    //create system logging thread
    

 
    for(int i=0;i<THREAD_NUMBER;i++)
    {
        if(pthread_create(&threadIDs[i],NULL,thread_functions[i],thread_parameters[i])!=0)
		    printf("Thread %d creation failed\n",i);

        else 
            printf("Thread created with ID %ld\n",threadIDs[i]);
    }

    logged_data_t * msg_temp,*msg_light;

    // Periodic requests for data from the sensor tasks
    while(1)
    {
        // Main request for temp sensor
        // Requester / Main Task

        //msg_temp = add_to_bdqueue(param1.temp_q);
        msg_temp = (logged_data_t*)bdqueue_next_empty_request(param1.temp_q);
        msg_temp->type = 1;
        bdqueue_done_writing_request(param1.temp_q);
        
        msg_temp = (logged_data_t*)bdqueue_next_response(param1.temp_q, false);
        
        printf("Q type:%d ts:%ld value:%lf \n",
            msg_temp->type,msg_temp->timestamp,msg_temp->value);

        // main request for light sensor
        msg_light = (logged_data_t*)bdqueue_next_empty_request(param1.light_q);
        msg_light->type = 2;
        bdqueue_done_writing_request(param1.light_q);
        
        msg_light = (logged_data_t*)bdqueue_next_response(param1.light_q, false);
        
        printf("Q type:%d ts:%ld value:%lf \n",
            msg_light->type,msg_light->timestamp,msg_light->value);
        
        
        sleep(1);
    }
    
    // Wait to join all children threads
    for(int i=0;i<THREAD_NUMBER;i++)
    {
        pthread_join(threadIDs[i],NULL);
    }

    return 0;
    
    #ifdef BBB
    // Test code
    // Code that will execute only on BBB
    //temp_sensor_raise_alert();

    
    //while(1)
    {
     
	    //sensors_test_print();	

    } 
    
     
    #endif
   
}
