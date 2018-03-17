/**
 * @brief Functions for thread tasks
 * 
 * @file tasks.c
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-16
 */

#include "tasks.h"

char* data_header_type_strings[] = {
    "Heartbeat",
    "Temperature",
    "Light",
};

logged_data_t * add_to_bdqueue(bdqueue * queue,data_header_type_t type)
{
    logged_data_t * msg;    
    msg = (logged_data_t*)bdqueue_next_empty_request(queue);
    msg->type = type;
    msg->req_time=time(NULL);
    bdqueue_done_writing_request(queue);
        
    msg = (logged_data_t*)bdqueue_next_response(queue, false);
    //bdqueue_done_reading_response(queue);
    
    return msg;
}


logged_data_t * bdqueue_response(bdqueue * queue,data_header_type_t type)
{
    logged_data_t * msg; 
    msg = (logged_data_t*)bdqueue_next_request(queue, false);
    msg->res_time=time(NULL); 

    switch (type)
    {
        case HEARTBEAT:
            msg->temperature.value=0;
            break;
        case TEMPERATURE:
            msg->temperature.value=get_temp(0);
            break;
        case LIGHT:
            msg->light.value=get_light();
            break;
    }

    bdqueue_done_reading_request_and_writing_response(queue);
    
    return msg;
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


void printQ(logged_data_t * msg)
{
    printf("req time:%ld resp time:%ld type:%s value:%lf \n",
            msg->req_time,msg->res_time,data_header_type_strings[msg->type],msg->light.value);
}


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
        
    while(p1->keep_thread_alive)
    {
        
        bdqueue_response(p1->temp_q,TEMPERATURE);

    }

    bdqueue_destroy(p1->temp_q);
    return NULL;
}


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
    
    while(p1->keep_thread_alive)
    {
        bdqueue_response(p1->light_q,LIGHT);
        
    }
    
     bdqueue_destroy(p1->light_q);
    
    return NULL;

}