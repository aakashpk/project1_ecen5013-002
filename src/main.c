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

    static char logfilename[] = "project1log.log";
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

    logger_struct logger;
    initialize_logger(&logger, logfilename);

    enable_logging_in_thread(&logger);

    log_printf("Main task started\n");

    // parameters to be passed during thread creation
    thread_param_t param1;

    if (thread_param_init(&param1) < 0)
        printf("Thread Param Init Failed\n");

    param1.logger = &logger;

    // Spawn the threads for the different tasks
    //

    pthread_t threadIDs[THREAD_NUMBER];

    // Array with functions for each of the tasks
    void *thread_functions[THREAD_NUMBER] = {socket_thread, temperature_task, light_task};

    // Array with parameters to be passed for each of these tasks
    // Maintain same order between the 2 arrays
    void *thread_parameters[THREAD_NUMBER] = {(void *)&param1, (void *)&param1, (void *)&param1};

    // TODO:
    //Create logger thread
    //create system logging thread

    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        if (pthread_create(&threadIDs[i], NULL, thread_functions[i], (void *)&param1) != 0)
            printf("Thread %d creation failed\n", i);

        else
            printf("Thread created with ID %ld\n", threadIDs[i]);
    }

    logged_data_t *msg;

    bdqueue *all_queues[] = {param1.temp_q, param1.light_q};
    char *queue_names[] = {"Temperature", "Light"};
    data_header_type_t queue_types[] = {TEMPERATURE, LIGHT};
    typedef enum {
        HBEAT,
        VALUE,
        THEME_MAX
    } rq_theme;

    // Periodic requests for data from the sensor tasks
    // TODO:Set this up with a timer , atleast the hearbeat has to be on timer
    while (1)
    {
        // Requester / Main Task

        // send all requests
        for (int i = 0; i < sizeof(all_queues) / sizeof(all_queues[0]); i++)
        {
            for (rq_theme t = HBEAT; t < THEME_MAX; t++)
            {
                msg = (logged_data_t*)bdqueue_next_empty_request(all_queues[i]);
                msg->type = (t == HBEAT) ? HEARTBEAT : queue_types[i];
                msg->req_time = time(NULL);

                // calls snprintf in thread context, so slight delay here.
                // Would be best to copy args to queue for logger to handle printf formatting.
                log_printf("Main Request: req %ld, source %s, msg type %s\n",
                    msg->req_time, queue_names[i], data_header_type_strings[msg->type]);

                bdqueue_done_writing_request(all_queues[i]);
            }
        }

        // Get data from all queues
        // Would ideally give each queue a timeout period, rather than non-block
        // TODO timeout
        for (int j = 0; j < 5; j++) // attempt to read a few times
        {
            for (int i = 0; i < sizeof(all_queues) / sizeof(all_queues[0]); i++)
            {
                while(msg = (logged_data_t*)bdqueue_next_response(all_queues[i], true))
                {
                    log_printf("Main Response: req %ld, rsp %ld, source %s, msg type %s, value %f\n",
                        msg->req_time, msg->res_time, queue_names[i], data_header_type_strings[msg->type],
                        msg->common.value);
                }
            }
            usleep(1000); // 1ms per read attempt
        }

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
