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

typedef struct
{
    // temp/light ?
    //req time

} socket_data_req_t;

typedef struct
{
    // value
    //response time

} socket_data_res_t;
