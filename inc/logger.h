#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <time.h>

#define DISP_TIME(t) ((t).tv_sec,(t).tv_nsec)

enum payload_type
{
    TEMPERATURE,
    LIGHT,
    SYSTEM
}payload_t;


typedef struct logdata_type
{
    struct timespec timestamp;
    payload_t dtype;
    uint32_t checksum;
    float value; // TODO: change to double ??

}logdata_t;


/**
 * @brief Function to create a log item
 * timestamp is calculated within the function
 * checksum is calculated using seperate function and update within
 * this function
 *
 * @param log variable
 * @param dtype
 * @param value
 *
 * @return pointer to location of created log
 */
logdata_t * createLog(logdata_t * log, payload_t dtype, float value);

uint8_t add_log(logdata_t log);
//TODO: Add another parameter with the pointer to queue where the log has to be added to, and change return type accordingly

uint32_t calc_checksum(float value,struct timespec timestamp, payload_t dtype);


logdata_t * log_initialize(size_t loglength);







#endif
