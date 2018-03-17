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

#include "logger.h"

#define THREAD_NUMBER 3

int main(int argc, char **argv)
{
    static char logfilename[]="project1log.log";

    logger_struct logger;
    initialize_logger(&logger, logfilename);

    enable_logging_in_thread(&logger);

    log_printf("Main says hi with number %d\n", 5);

    while(1);

    extern char *optarg;
    extern int optind;
    int optret;

    // Get Log file name from command line
    // If not proper format use a default one
    optret = getopt(argc, argv, "f:");

    if (argc < 3 || optret != 'f')
    {
        printf("Usage is project1 -f logfilename\n \
        File option not proper, using project1log.log\n");
    }
    else
        strcpy(logfilename, optarg);

    printf("Log Filename is %s\n", logfilename);

    // parameters to be passed during thread creation
    thread_param_t param1;

    if (thread_param_init(&param1) < 0)
        printf("Thread Param Init Failed\n");

    // Copy log file into prameter structure
    param1.logfile_name = malloc(strlen(logfilename));
    strcpy(param1.logfile_name, logfilename);

    // Spawn the threads for the different tasks
    //

    pthread_t threadIDs[THREAD_NUMBER];

    // Array with functions for each of the tasks
    void *thread_functions[THREAD_NUMBER] = {socket_thread, temperature_task, light_task};

    // TODO:Create logger thread

    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        if (pthread_create(&threadIDs[i], NULL, thread_functions[i], (void *)&param1) != 0)
            printf("Thread %d creation failed\n", i);

        else
            printf("Thread created with ID %ld\n", threadIDs[i]);
    }

    logged_data_t *msg_temp, *msg_light, *msg_hb;

    // Periodic requests for data from the sensor tasks
    // TODO:Set this up with a timer , atleast the hearbeat has to be on timer
    while (1)
    {
        //heartbeat
        msg_hb = add_to_bdqueue(param1.temp_q, HEARTBEAT);
        bdqueue_done_reading_response(param1.temp_q);
        printQ(msg_hb);

        // Main request for temp sensor
        msg_temp = add_to_bdqueue(param1.temp_q, TEMPERATURE);
        printQ(msg_temp);

        // main request for light sensor
        msg_light = add_to_bdqueue(param1.light_q, LIGHT);
        printQ(msg_light);

        sleep(1);
    }

    // Wait to join all children threads
    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        pthread_join(threadIDs[i], NULL);
    }

    free(param1.temp_q);
    free(param1.light_q);

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
