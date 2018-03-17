/**
 * @brief Functions for thread tasks
 * 
 * @file tasks.c
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-16
 */

#include "tasks.h"

logged_data_t * add_to_bdqueue(bdqueue ** queue)
{
    logged_data_t * msg;    
    msg = (logged_data_t*)bdqueue_next_empty_request(*queue);
    msg->type = 1;
    bdqueue_done_writing_request(*queue);
        
    msg = (logged_data_t*)bdqueue_next_response(*queue, false);
    //bdqueue_done_reading_response(param1.temp_q);
}

int thread_param_init(thread_param_t *param)
{
    if(queue_init(&(param->temp_q))<0)
         return -1;

    if(queue_init(&(param->light_q))<0)
        return -1;

    // Set thread alive to 1,
    // All threads close out when this goes to 0
    param->keep_thread_alive=1;

    return 0;
}

int queue_init(bdqueue ** queue)
{
    *queue=malloc(sizeof(bdqueue));
    
    if(*queue==NULL) return -1;

    if (bdqueue_init(*queue, sizeof(logged_data_t), 5) == QUEUE_FAILURE)
    {
        printf("Q init failed\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Initializes the light sensor
 * this will also do the startup test to see if I2C comms
 * is working
 * and sets up timer to periodically log temperature
 * 
 * 
 */
void * temperature_task(void * thread_param)
{

    #ifdef BBB
    if(temp_sensor_init()==0) LOG_STR("Temp Sensor Self Test Done\n");
    else 
    {
        LOG_STR("Temp Sensor Init Failed\n");
        exit(1);
    }
    #endif

    thread_param_t * p1= (thread_param_t*)thread_param;
    logged_data_t *msg;
    
    while(p1->keep_thread_alive)
    {
        // Responder / Sensor Task
        msg = (logged_data_t*)bdqueue_next_request(p1->temp_q, false);
        //msg->type= 1;
        msg->timestamp=time(NULL);
        msg->value=get_temp(0);

        bdqueue_done_reading_request_and_writing_response(p1->temp_q);

        //printf("ts:[%d] val:%lf type:T \n",msg->timestamp,msg->value);
        //sleep(1);
    }
    return NULL;
}

/**
 * @brief Initializes the light sensor
 * Does the startup test, and checks the sensor id
 * Sets up periodic timer to log light data 
 * 
 */
void * light_task(void * thread_param)
{

    #ifdef BBB
    if(light_sensor_init()==0) LOG_STR("Light Sensor Self Test Done\n");
    else 
    {
        LOG_STR("Light Sensor Init Failed\n");
        exit(1);
    }
    #endif

    

    thread_param_t * p1= (thread_param_t*)thread_param;
    logged_data_t *msg;

    while(p1->keep_thread_alive)
    {
        msg = (logged_data_t*)bdqueue_next_request(p1->light_q, false);
        //msg->type= 1;
        msg->timestamp=time(NULL);
        msg->value=get_light();

        bdqueue_done_reading_request_and_writing_response(p1->light_q);

        //printf("ts:[%d] val:%lf type:L  \n",msg->timestamp,msg->value);

        //sleep(1);
    }
    return NULL;

}