#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>

#define SOCKET_NAME "mysocket"

typedef enum {
    HEARTBEAT,
    TEMPERATURE,
    LIGHT,
} data_header_type_t; // TODO would rather call this message_header_t

typedef enum {
    SOCKET,
    MAIN,
} message_origin_t;

extern char *data_header_type_strings[];

typedef struct
{
    data_header_type_t type;

    // TODO: Change to timerspec struct for precise time
    long req_time;
    long res_time;

    message_origin_t origin;

    union {
        struct
        {
            float value;
        } temperature;
        struct
        {
            float value;
        } light;
        struct
        {
            float value;
        } common; // Easy way to access both sensor values
    };
} logged_data_t;