/**
 * @brief Socket client to make API requests to main
 * 
 * @file socketclient.h
 * @author Aakash Kumar
 * @author Miles Frain
 * @date 2018-03-12
 */
#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

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

#define SOCKET_NAME "mysocket"

/*
Functions for
    Connecting to socket server
    sending requests
    receiving responses
*/

/**
 * @brief Open socket client file
 * and return file descriptor
 * 
 * @return int socket flie descriptor
 */
int create_socket_client();

/**
 * @brief Connect to socket server,
 * and return socket client file descriptor
 * 
 * @param socket_fd 
 * @return int 0 if connection successful 
 * -1 if unsuccessful
 */
int socket_connect(int socket_fd);

#endif